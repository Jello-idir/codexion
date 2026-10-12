/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 16:17:39 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

static int	start_coders(t_cnf *cnf, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < cnf->args[N_CODERS])
	{
		if (pthread_create(threads + i, NULL, coderjob, cnf->coders[i]))
			break ;
		i++;
	}
	return (i);
}

static void	join_threads(pthread_t *threads, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
}

int	run_simulation(t_cnf *cnf, pthread_t *threads)
{
	pthread_t	monitor_id;
	int			count;
	int			failed;

	pthread_mutex_lock(cnf->simmutex);
	count = start_coders(cnf, threads);
	failed = 1;
	if (count == cnf->args[N_CODERS]
		&& clock_gettime(CLOCK_MONOTONIC, cnf->start_time) == 0)
		failed = pthread_create(&monitor_id, NULL, monitor, cnf);
	cnf->stopped = (failed != 0);
	pthread_mutex_unlock(cnf->simmutex);
	if (failed)
		wake_dongles(cnf);
	else
		pthread_join(monitor_id, NULL);
	join_threads(threads, count);
	return (failed);
}
