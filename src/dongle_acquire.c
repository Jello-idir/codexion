/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_acquire.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 23:30:00 by aixel             #+#    #+#             */
/*   Updated: 2026/10/09 23:30:00 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

static int	take_dongles(t_coder *coder)
{
	struct timespec	now;
	int				ready;

	lock_dongles(coder);
	clock_gettime(CLOCK_MONOTONIC, &now);
	ready = dongles_available(coder)
		&& ts_to_ns(&now) >= dongles_ready_at(coder);
	if (ready)
	{
		coder->ldongle->available = 0;
		coder->rdongle->available = 0;
	}
	unlock_dongles(coder);
	return (ready);
}

static int	wait_alone(t_coder *coder)
{
	lock_dongles(coder);
	coder->ldongle->available = 0;
	unlock_dongles(coder);
	log_locked(coder, get_simulation_time(coder->conf->start_time),
		"has taken a dongle");
	while (!coder->conf->stopped)
		pthread_cond_wait(coder->conf->changed, coder->conf->simmutex);
	lock_dongles(coder);
	coder->ldongle->available = 1;
	clock_gettime(CLOCK_MONOTONIC, &coder->ldongle->released_at);
	unlock_dongles(coder);
	return (0);
}

static int	queue_pair(t_coder *coder)
{
	int	acquired;

	queue_requests(coder);
	acquired = 0;
	while (!coder->conf->stopped && !acquired)
	{
		acquired = take_dongles(coder);
		if (!acquired)
			wait_for_dongles(coder);
	}
	lock_dongles(coder);
	heapremove(coder->ldongle->heap, coder);
	heapremove(coder->rdongle->heap, coder);
	unlock_dongles(coder);
	return (acquired);
}

int	get_dongles(t_coder *coder)
{
	int	acquired;

	pthread_mutex_lock(coder->conf->simmutex);
	acquired = 0;
	if (!coder->conf->stopped)
	{
		if (coder->ldongle == coder->rdongle)
			acquired = wait_alone(coder);
		else
			acquired = queue_pair(coder);
	}
	pthread_mutex_unlock(coder->conf->simmutex);
	return (acquired);
}
