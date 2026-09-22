/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 18:03:06 by aixel             #+#    #+#             */
/*   Updated: 2026/09/22 21:52:39 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

static void    swap(t_coder **a, t_coder **b)
{
    t_coder *tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

static void    heappify(t_heap *heap, int index)
{
    int small;
    int left;
    int right;

    small = index;
    left = small * 2 + 1;
    right = small * 2 + 2;
    if (heap->size > left && heap->coders[left]->death_time < heap->coders[small]->death_time)
        small = left;
    if (heap->size > right && heap->coders[right]->death_time < heap->coders[small]->death_time)
        small = right;
    if (small != index) {
        swap(heap->coders + small, heap->coders + index);
        heappify(heap, small);
    }
}

void    heappush(t_heap *heap, t_coder *newcoder)
{
    int index;

    if (heap->size >= heap->capacity)
    {
        fprintf(stderr, "heap error!");
        return;
    }
    heap->size++;
    index = heap->size - 1;
    heap->coders[index] = newcoder;
    while (index > 0 && heap->coders[index]->death_time < heap->coders[(index - 1) / 2]->death_time)
    {
        swap(heap->coders + index, heap->coders + ((index - 1) / 2));
        index = (index - 1) / 2;
    }
}

t_coder *heappop(t_heap *heap)
{
    t_coder *popped;

    if (heap->size <= 0)
        return NULL;
    popped = heap->coders[0];
    swap(heap->coders, heap->coders + heap->size - 1);
    heap->size--;
    heappify(heap, 0);
    return popped;
}

t_heap *heap_init(int capacity)
{
    t_heap *heap;

    if (capacity <= 0)
        return NULL;
    heap = malloc(sizeof(t_heap));
    if (!heap)
        return NULL;
    heap->size = 0;
    heap->capacity = capacity;
    heap->coders = malloc(sizeof(int) * capacity);
    if (!heap->coders) {
        free(heap);
        return NULL;
    }
    return heap;
}
