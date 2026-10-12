/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   startup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 23:30:00 by aixel             #+#    #+#             */
/*   Updated: 2026/10/09 23:30:00 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

/* Register odd IDs, then even IDs, before any coder can request again. */
static int	startup_position(t_coder *coder)
{
	if (coder->id % 2)
		return ((coder->id - 1) / 2);
	return ((coder->conf->args[N_CODERS] + coder->id - 1) / 2);
}

/* Spread first compiles so adjacent coders are one reuse interval apart. */
static unsigned long long	startup_time(t_coder *coder)
{
	unsigned long long	cycle;
	unsigned long long	delay;
	int					count;

	count = coder->conf->args[N_CODERS];
	cycle = (unsigned long long)coder->conf->args[COMPILE_T]
		+ coder->conf->args[D_COOLDOWN];
	if (count % 2)
		delay = cycle * startup_position(coder) / (count / 2);
	else
		delay = cycle * (coder->id % 2 == 0);
	return (ts_to_ns(coder->conf->start_time) + delay * 1000000ULL);
}

static void	wait_startup(t_coder *coder)
{
	struct timespec		now;
	unsigned long long	end;

	while (!coder->conf->stopped
		&& coder->conf->queued < coder->conf->args[N_CODERS])
		pthread_cond_wait(coder->conf->changed, coder->conf->simmutex);
	end = startup_time(coder);
	clock_gettime(CLOCK_MONOTONIC, &now);
	while (!coder->conf->stopped && ts_to_ns(&now) < end)
	{
		wait_cooldown(coder->conf, end);
		clock_gettime(CLOCK_MONOTONIC, &now);
	}
}

void	queue_requests(t_coder *coder)
{
	int	first;

	first = coder->ncompile == coder->conf->args[N_COMPILES];
	while (first && !coder->conf->stopped
		&& coder->conf->queued != startup_position(coder))
		pthread_cond_wait(coder->conf->changed, coder->conf->simmutex);
	if (coder->conf->stopped)
		return ;
	lock_dongles(coder);
	heappush(coder->ldongle->heap, coder);
	heappush(coder->rdongle->heap, coder);
	unlock_dongles(coder);
	if (first)
	{
		coder->conf->queued++;
		pthread_cond_broadcast(coder->conf->changed);
		wait_startup(coder);
	}
}
