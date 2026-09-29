/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 13:24:02 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/28 00:02:58 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#include <time.h>

unsigned long    ts_to_ms(struct timespec *ts)
{
    return (ts->tv_sec * 1000 + ts->tv_nsec / 1000000);
}

void print_timestamp(struct timespec *start_time)
{
    struct timespec now;
    now.tv_nsec = 0;
    now.tv_sec = 0;

    clock_gettime(CLOCK_MONOTONIC, &now);
    printf("%05lu", ts_to_ms(&now) - ts_to_ms(start_time));
}

void display_conf(int *simconf)
{
	char *txt[] = {
		"number_of_coders",
		"time_to_burnout",
		"time_to_compile",
		"time_to_debug",
		"time_to_refactor",
		"number_of_compiles",
		"dongle_cooldown",
		"scheduler",
	};
	printf("------------------------------\n");
	printf("\033[2m");
	for (int i = 0; i < 7; i++)
		printf("- %-20s : %i\n", txt[i], simconf[i]);
	char *type = "unknown";
	if (simconf[7] == FIFO)
		type = "fifo";
	else if (simconf[7] == EDF)
		type = "edf";
	printf("- %-20s : %s\n", txt[7], type);
	printf("\033[0m");
	printf("------------------------------\n");
}

void nspace(int n) {
    for (int i = 0; i < n; i++)
        printf(" ");
}

int count_levels_in_tree(int size) {
    int index = 0;
    int cnt = 1;

    if (!size)
        return 0;
    while (index < size - 1)
    {
        index = (index + 1) * 2;
        cnt++;
    }
    return cnt;
}

int power(int a, int p)
{
    int result = 1;

    while (p)
    {
        result *= a;
        p--;
    }
    return result;
}

void heap_print(t_heap *heap) {
    int curr_level = 0;
    int max_level = count_levels_in_tree(heap->size);
    int nodes_in_level;
    int space;
    int first = 1;
    int field_w = 2;
    int base = (field_w + 1) << (max_level - 1);

    nodes_in_level = power(2, curr_level);
    printf("------------------------------------\n");
    for (int i = 0; i < heap->size; i++) {
        space = base >> curr_level;
        if (first) {
            first = 0;
            nspace(space / 2);
        } else {
            nspace(space - field_w > 0 ? space - field_w : 0);
        }
        if (heap->coders[i]->death_time == 99)
            printf("\033[31m%0*d\033[0m", field_w, heap->coders[i]->death_time);
        else if (heap->coders[i]->death_time == 1)
            printf("\033[33m%0*d\033[0m", field_w, heap->coders[i]->death_time);
        else
            printf("%0*d", field_w, heap->coders[i]->death_time);
        nodes_in_level--;
        if (nodes_in_level == 0 && i + 1 < heap->size)
        {
            printf("\n");
            curr_level += 1;
            nodes_in_level = power(2, curr_level);
            first = 1;
        }
    }
    printf("\n------------------------------------\n");
}
