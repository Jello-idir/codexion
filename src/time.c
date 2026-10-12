/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 16:15:27 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

unsigned long long	ts_to_ns(struct timespec *ts)
{
	return ((unsigned long long)ts->tv_sec * 1000000000ULL + ts->tv_nsec);
}

unsigned long long	get_simulation_time(struct timespec *start_time)
{
	struct timespec	now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	return ((ts_to_ns(&now) - ts_to_ns(start_time)) / 1000000ULL);
}

int	simulation_sleep(t_cnf *conf, int duration)
{
	struct timespec		now;
	unsigned long long	end;
	unsigned long long	remaining;

	clock_gettime(CLOCK_MONOTONIC, &now);
	end = ts_to_ns(&now) + (unsigned long long)duration * 1000000ULL;
	while (!isstopped(conf))
	{
		clock_gettime(CLOCK_MONOTONIC, &now);
		if (ts_to_ns(&now) >= end)
			return (1);
		remaining = (end - ts_to_ns(&now) + 999) / 1000;
		if (remaining > 500)
			remaining = 500;
		usleep(remaining);
	}
	return (0);
}
