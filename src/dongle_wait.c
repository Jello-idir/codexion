/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_wait.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 10:51:01 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

static unsigned long long	ready_at(t_dongle *dongle)
{
	if (ts_to_ns(&dongle->released_at) == 0)
		return (0);
	return (ts_to_ns(&dongle->released_at) + dongle->cooldown * 1000000ULL);
}

unsigned long long	dongles_ready_at(t_coder *coder)
{
	unsigned long long	left;
	unsigned long long	right;

	left = ready_at(coder->ldongle);
	right = ready_at(coder->rdongle);
	if (left > right)
		return (left);
	return (right);
}

void	wait_cooldown(t_cnf *conf, unsigned long long end)
{
	struct timespec		now;
	struct timespec		timeout;

	clock_gettime(CLOCK_MONOTONIC, &now);
	if (end <= ts_to_ns(&now))
		return ;
	end -= ts_to_ns(&now);
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += end / 1000000000ULL;
	timeout.tv_nsec += end % 1000000000ULL;
	timeout.tv_sec += timeout.tv_nsec / 1000000000L;
	timeout.tv_nsec %= 1000000000L;
	pthread_cond_timedwait(conf->changed, conf->simmutex, &timeout);
}

void	wait_for_dongles(t_coder *coder)
{
	unsigned long long	end;
	int					available;

	lock_dongles(coder);
	end = dongles_ready_at(coder);
	available = dongles_available(coder);
	unlock_dongles(coder);
	if (!available)
		pthread_cond_wait(coder->conf->changed, coder->conf->simmutex);
	else
		wait_cooldown(coder->conf, end);
}
