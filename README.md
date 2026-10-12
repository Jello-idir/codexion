*This project has been created as part of the 42 curriculum by aait-idi.*

# Codexion

## Description

Codexion is a C simulation inspired by the dining philosophers problem. Each
coder runs in a thread and cycles through compiling, debugging, and refactoring.
Compiling requires two shared dongles, which need a cooldown after use.

The goal is to manage shared resources safely. The simulation stops when a coder
burns out or everyone finishes the required compilations. Starting a compilation
resets that coder's burnout timer.

## Instructions

You need `make`, a C compiler, and POSIX threads. Build with:

```sh
make
```

Run with all eight arguments:

```text
./codexion coders burnout compile debug refactor compiles cooldown scheduler
```

| Argument | Meaning |
| --- | --- |
| `coders` | Number of coders and dongles. |
| `burnout` | Time allowed before the first compilation or between compilation starts. |
| `compile` | Compilation duration. |
| `debug` | Debugging duration. |
| `refactor` | Refactoring duration. |
| `compiles` | Required compilations per coder. |
| `cooldown` | Delay before a released dongle can be reused. |
| `scheduler` | `fifo` for arrival order, or `edf` for earliest burnout deadline. |

Times are in **milliseconds**. Numbers must be positive integers fitting in a C
`int`, except cooldown, which may be zero.

Example: four coders, three compilations each, using FIFO:

```sh
./codexion 4 1000 100 100 100 3 50 fifo
```

Logs show `elapsed_ms coder_id action`. Invalid arguments print `Error`.
Use `make clean` to remove object files, `make fclean` to also remove the
executable, or `make re` to rebuild.

## Blocking cases handled

- **Deadlocks:** both dongles are reserved together, and their mutexes are locked
  in ID order. This breaks hold-and-wait and circular wait, two of Coffman's four
  deadlock conditions. Mutual exclusion and no preemption remain.
- **Starvation:** FIFO preserves request order; EDF prioritizes the nearest
  deadline. Staggered starts reduce contention. Tight deadlines can still cause
  burnout.
- **Cooldowns:** release timestamps and timed waits prevent early dongle reuse.
- **Burnout detection:** the monitor checks monotonic deadlines, sleeping 500 µs
  between scans. Shared locks protect deadline updates. Actual detection time
  depends on thread scheduling.
- **Logging:** messages are printed one at a time, with no activity logged after
  the burnout message.
- **Single coder and shutdown:** one coder waits for burnout because it cannot
  get two dongles. Shutdown wakes waiting threads and joins all started threads
  before cleanup.

## Thread synchronization mechanisms

- **`pthread_mutex_t`:** `simmutex` protects deadlines, compilation counts, startup
  state, and the stop flag. Each dongle has a mutex protecting its availability,
  cooldown timestamp, and request queue. `radio` serializes logs.
- **`pthread_cond_t`:** `changed` lets coders wait for startup, dongles, cooldowns,
  or shutdown. Waiting releases `simmutex`; waking reacquires it. Conditions are
  checked again after every wakeup. No custom event implementation is used.

For example, checking and reserving both dongles happens under the same locks,
so two neighbors cannot claim the same dongle. Coders update their deadlines and
compilation counts under `simmutex`; the monitor reads them under that same lock.
Releasing dongles or stopping the simulation broadcasts `changed` to wake waiters.

## Resources

- POSIX mutexes `man page`
- POSIX condition variables `man page`

AI helped to find bugs, fix code formatting (norminette) and write this readme.
