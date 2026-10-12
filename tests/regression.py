#!/usr/bin/env python3
"""Check behavior, arbitration, and the repeated correction.txt benchmarks.

Run everything: python3 tests/regression.py [binary]
Short checks:   python3 tests/regression.py [binary] --quick
Benchmarks:    python3 tests/regression.py [binary] --correction --repeat 3
"""

import argparse
from collections import Counter
from pathlib import Path
import re
import subprocess
import tempfile
import time


ROOT = Path(__file__).resolve().parents[1]
EVENT = re.compile(
    r"(\d+) (\d+) "
    r"(has taken a dongle|is compiling|is debugging|is refactoring|burned out)"
)


def run(binary, args):
    start = time.monotonic()
    result = subprocess.run(
        [str(binary), *map(str, args)], capture_output=True, text=True, timeout=90
    )
    return result, (time.monotonic() - start) * 1000


def check_events(args, events):
    count, burnout, compile_ms, debug_ms, refactor_ms, _, cooldown, _ = args
    last_compile = {}
    phase = {}
    reusable_at = {}
    for i, (stamp, coder, event) in enumerate(events):
        assert 1 <= coder <= count, (args, events[i])
        if event == "is compiling":
            assert i >= 2 and all(
                entry[1:] == (coder, "has taken a dongle")
                for entry in events[i - 2:i]
            ), (args, "compile must immediately follow two takes", events[max(0, i - 3):i + 1])
            assert count > 1, (args, "one dongle cannot be used twice")
            assert stamp < last_compile.get(coder, 0) + burnout, (args, events[i])
            # Adjacent coders share a dongle. Reuse must wait for both
            # the previous compilation and the cooldown.
            for dongle in (coder, coder % count + 1):
                assert stamp >= reusable_at.get(dongle, 0), (
                    args, "dongle overlap or early cooldown", dongle, events[i]
                )
                reusable_at[dongle] = stamp + compile_ms + cooldown
            if coder in phase:
                assert phase[coder][1] == "is refactoring", (args, events[i])
                assert stamp - phase[coder][0] >= refactor_ms, (args, events[i])
            last_compile[coder] = stamp
        elif event == "is debugging":
            assert phase[coder][1] == "is compiling", (args, events[i])
            assert stamp - phase[coder][0] >= compile_ms, (args, events[i])
        elif event == "is refactoring":
            assert phase[coder][1] == "is debugging", (args, events[i])
            assert stamp - phase[coder][0] >= debug_ms, (args, events[i])
        elif event == "burned out":
            delay = stamp - (last_compile.get(coder, 0) + burnout)
            assert 0 <= delay <= 10, (args, "burnout delay in ms", delay)
        if event != "has taken a dongle":
            phase[coder] = (stamp, event)


def simulation(binary, args, burnout):
    result, elapsed = run(binary, args)
    args = [*map(int, args[:7]), args[7]]
    assert result.returncode == 0, (args, result.returncode, result.stderr)
    assert not result.stderr, (args, result.stderr)
    matches = [EVENT.fullmatch(line) for line in result.stdout.splitlines()]
    assert matches and all(matches), (args, result.stdout)
    events = [(int(m[1]), int(m[2]), m[3]) for m in matches]
    timestamps = [stamp for stamp, _, _ in events]
    assert timestamps == sorted(timestamps), (args, events)
    deaths = [i for i, event in enumerate(events) if event[2] == "burned out"]
    if burnout:
        assert deaths == [len(events) - 1], (args, result.stdout)
    else:
        assert not deaths, (args, result.stdout)
        compiles = Counter(coder for _, coder, event in events
                           if event == "is compiling")
        expected = {coder: args[5] for coder in range(1, args[0] + 1)}
        assert compiles == expected, (args, compiles)
        last_start = max(stamp for stamp, _, event in events
                         if event == "is compiling")
        assert elapsed >= last_start + args[2], (args, elapsed, last_start)
    if args[0] == 1:
        assert [event[2] for event in events] == [
            "has taken a dongle", "burned out"
        ], (args, events)
    else:
        takes = sum(event == "has taken a dongle" for _, _, event in events)
        compiles = sum(event == "is compiling" for _, _, event in events)
        assert takes == compiles * 2, (args, takes, compiles)
    check_events(args, events)
    return events


def behavior_tests(binary):
    count = 0
    for policy in ("fifo", "edf"):
        cases = [
            ([1, 800, 200, 200, 200, 10, 0, policy], True),
            ([5, 500, 200, 200, 200, 10, 0, policy], True),
            ([2, 2000, 60, 60, 60, 3, 60, policy], False),
            ([5, 3000, 60, 60, 60, 4, 120, policy], False),
            ([2, 200, 120, 60, 60, 1, 0, policy], False),
            ([2, 800, 60, 60, 60, 2, 2147483647, policy], True),
            ([2, 80, 2147483647, 60, 60, 2, 60, policy], True),
            ([2, 200, 60, 2147483647, 60, 2, 0, policy], True),
            ([2, 250, 60, 60, 2147483647, 2, 0, policy], True),
            ([199, 5000, 60, 60, 60, 3, 60, policy], False),
            ([200, 5000, 60, 60, 60, 3, 60, policy], False),
            ([31, 80, 200, 60, 60, 2, 1000, policy], True),
            ([2, 1000, 60, 60, 60, 2, "+0", policy], False),
            ([2, 1000, 60, 60, 60, 2, "000", policy], False),
        ]
        for args, burnout in cases:
            simulation(binary, args, burnout)
            count += 1
        # With two coders every handoff reuses both dongles.
        events = simulation(binary, [2, 3000, 60, 60, 60, 3, 120, policy], False)
        starts = [stamp for stamp, _, event in events if event == "is compiling"]
        assert all(b - a >= 180 for a, b in zip(starts, starts[1:])), starts
        count += 1
    return count


def invalid_input_tests(binary):
    valid = [2, 1000, 60, 60, 60, 1, 0, "fifo"]
    cases = [[], valid[:-1], valid + [1]]
    for index in range(7):
        for value in ("-1", "abc", "", "+", "2147483648", "1.5", " 1", "1 "):
            args = valid.copy()
            args[index] = value
            cases.append(args)
        if index != 6:
            args = valid.copy()
            args[index] = 0
            cases.append(args)
    for policy in ("FIFO", "EDF", "lifo", "invalid", ""):
        cases.append(valid[:-1] + [policy])
    for args in cases:
        result, _ = run(binary, args)
        assert result.returncode == 1 and result.stderr == "Error\n", (args, result)
        assert not result.stdout, (args, result.stdout)
    return len(cases)


def scheduler_tests():
    source = r'''
#include "codexion.h"
#include <assert.h>
int main(void)
{
    t_coder first = {.death_time = 9000};
    t_coder urgent = {.death_time = 3000};
    t_heap *queue;
    for (int policy = FIFO; policy <= EDF; policy++) {
        queue = heap_init(policy);
        assert(queue);
        heappush(queue, &first);
        heappush(queue, &urgent);
        assert(heappop(queue) == (policy == FIFO ? &first : &urgent));
        assert(heappop(queue) == (policy == FIFO ? &urgent : &first));
        assert(heappop(queue) == NULL);
        free(queue);
    }
    queue = heap_init(EDF);
    assert(queue);
    first.death_time = urgent.death_time;
    heappush(queue, &first);
    heappush(queue, &urgent);
    assert(heappop(queue) == &first);
    assert(heappop(queue) == &urgent);
    heappush(queue, &first);
    heappush(queue, &urgent);
    heapremove(queue, &first);
    assert(heappop(queue) == &urgent);
    assert(heappeek(queue) == NULL);
    free(queue);
    return 0;
}
'''
    with tempfile.TemporaryDirectory(prefix="codexion-queue-") as directory:
        directory = Path(directory)
        source_path = directory / "queue.c"
        source_path.write_text(source)
        binary = directory / "queue"
        subprocess.run([
            "cc", "-Wall", "-Wextra", "-Werror", "-pthread",
            "-I", str(ROOT / "header"), str(source_path),
            str(ROOT / "src/heap.c"), "-o", str(binary)
        ], check=True)
        subprocess.run([str(binary)], check=True, timeout=5)
    return 4


def correction_tests(binary, repeats):
    cases = [
        ([1, 800, 200, 200, 200, 10, 0, "fifo"], True),
        ([5, 2000, 200, 200, 200, 10, 0, "fifo"], False),
        ([5, 2000, 200, 200, 200, 7, 0, "edf"], False),
        ([5, 500, 200, 200, 200, 10, 0, "fifo"], True),
        ([5, 3000, 200, 200, 200, 10, 400, "fifo"], False),
        ([5, 3000, 200, 200, 200, 10, 800, "fifo"], False),
        ([5, 3000, 200, 200, 200, 10, 800, "edf"], False),
        ([5, 3000, 200, 200, 200, 10, 400, "edf"], False),
    ]
    for args, burnout in cases:
        for _ in range(repeats):
            simulation(binary, args, burnout)
        print(f"PASS {repeats}x: {' '.join(map(str, args))}", flush=True)
    return len(cases) * repeats


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("binary", nargs="?", type=Path, default=ROOT / "codexion")
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--quick", action="store_true", help="skip long benchmarks")
    mode.add_argument("--correction", action="store_true", help="only benchmarks")
    parser.add_argument("--repeat", type=int, default=3)
    options = parser.parse_args()
    if options.repeat < 1:
        parser.error("--repeat must be at least 1")
    binary = options.binary.resolve()
    count = 0
    if not options.correction:
        count += behavior_tests(binary) + invalid_input_tests(binary) + scheduler_tests()
        print(f"Passed {count} behavior, input, and queue checks", flush=True)
    if not options.quick:
        count += correction_tests(binary, options.repeat)
    print(f"Passed {count} checks total", flush=True)


if __name__ == "__main__":
    main()
