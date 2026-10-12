/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 16:15:18 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

int	isstopped(t_cnf *conf)
{
	int	stopped;

	pthread_mutex_lock(conf->simmutex);
	stopped = conf->stopped;
	pthread_mutex_unlock(conf->simmutex);
	return (stopped);
}

void	log_locked(t_coder *coder, unsigned long long now, char *what)
{
	pthread_mutex_lock(coder->conf->radio);
	printf("%llu %u %s\n", now, coder->id, what);
	pthread_mutex_unlock(coder->conf->radio);
}

int	radio_message(t_coder *coder, char *what)
{
	int	running;

	pthread_mutex_lock(coder->conf->simmutex);
	running = !coder->conf->stopped;
	if (running)
		log_locked(coder, get_simulation_time(coder->conf->start_time), what);
	pthread_mutex_unlock(coder->conf->simmutex);
	return (running);
}
