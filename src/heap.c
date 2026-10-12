/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 10:51:01 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

void	heappush(t_heap *heap, t_coder *newcoder)
{
	int			index;
	t_request	tmp;

	if (heap->size >= 2)
	{
		fprintf(stderr, "heap error!\n");
		return ;
	}
	index = heap->size++;
	heap->requests[index].coder = newcoder;
	heap->requests[index].deadline = newcoder->death_time;
	if (heap->size == 2 && heap->policy == EDF
		&& heap->requests[1].deadline < heap->requests[0].deadline)
	{
		tmp = heap->requests[0];
		heap->requests[0] = heap->requests[1];
		heap->requests[1] = tmp;
	}
}

t_coder	*heappop(t_heap *heap)
{
	t_coder	*popped;

	if (heap->size <= 0)
		return (NULL);
	popped = heap->requests[0].coder;
	heap->size--;
	if (heap->size == 1)
		heap->requests[0] = heap->requests[1];
	return (popped);
}

t_coder	*heappeek(t_heap *heap)
{
	if (heap->size <= 0)
		return (NULL);
	return (heap->requests[0].coder);
}

void	heapremove(t_heap *heap, t_coder *coder)
{
	int	i;

	i = 0;
	while (i < heap->size && heap->requests[i].coder != coder)
		i++;
	if (i == heap->size)
		return ;
	heap->size--;
	while (i < heap->size)
	{
		heap->requests[i] = heap->requests[i + 1];
		i++;
	}
}

t_heap	*heap_init(int policy)
{
	t_heap	*heap;

	if (policy != FIFO && policy != EDF)
		return (NULL);
	heap = malloc(sizeof(*heap));
	if (!heap)
		return (NULL);
	heap->size = 0;
	heap->policy = policy;
	return (heap);
}
