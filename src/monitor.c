/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 11:15:28 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

static void	check_simulation(t_cnf *cnf)
{
	int					i;
	int					finished;
	unsigned long long	now;

	i = 0;
	finished = 0;
	now = get_simulation_time(cnf->start_time);
	while (i < cnf->args[N_CODERS])
	{
		if (cnf->coders[i]->ncompile == 0)
			finished++;
		else if (now >= cnf->coders[i]->death_time)
		{
			cnf->stopped = 1;
			log_locked(cnf->coders[i], now, "burned out");
			return ;
		}
		i++;
	}
	if (finished == cnf->args[N_CODERS])
		cnf->stopped = 1;
}

void	*monitor(void *arg)
{
	t_cnf	*cnf;
	int		stopped;

	cnf = (t_cnf *)arg;
	while (1)
	{
		pthread_mutex_lock(cnf->simmutex);
		if (!cnf->stopped)
			check_simulation(cnf);
		stopped = cnf->stopped;
		pthread_mutex_unlock(cnf->simmutex);
		if (stopped)
			break ;
		usleep(500);
	}
	wake_dongles(cnf);
	return (NULL);
}
