/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 10:51:01 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

static int	start_compile(t_coder *coder)
{
	unsigned long long	now;
	int					started;

	pthread_mutex_lock(coder->conf->simmutex);
	now = get_simulation_time(coder->conf->start_time);
	started = !coder->conf->stopped && now < coder->death_time;
	if (started)
	{
		coder->death_time = now + coder->conf->args[BURNOUT_T];
		log_locked(coder, now, "has taken a dongle");
		log_locked(coder, now, "has taken a dongle");
		log_locked(coder, now, "is compiling");
	}
	pthread_mutex_unlock(coder->conf->simmutex);
	return (started);
}

static int	finish_compile(t_coder *coder)
{
	int	more;

	pthread_mutex_lock(coder->conf->simmutex);
	more = 0;
	if (!coder->conf->stopped
		&& get_simulation_time(coder->conf->start_time) < coder->death_time)
	{
		coder->ncompile--;
		more = coder->ncompile > 0;
	}
	pthread_mutex_unlock(coder->conf->simmutex);
	return (more);
}

static int	compile(t_coder *coder)
{
	int	more;

	if (!get_dongles(coder))
		return (0);
	more = 0;
	if (start_compile(coder)
		&& simulation_sleep(coder->conf, coder->conf->args[COMPILE_T]))
		more = finish_compile(coder);
	release_dongles(coder);
	return (more);
}

void	*coderjob(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!isstopped(coder->conf))
	{
		if (!compile(coder))
			break ;
		if (!radio_message(coder, "is debugging")
			|| !simulation_sleep(coder->conf, coder->conf->args[DEBUG_T]))
			break ;
		if (!radio_message(coder, "is refactoring")
			|| !simulation_sleep(coder->conf, coder->conf->args[REFACTOR_T]))
			break ;
	}
	return (NULL);
}
