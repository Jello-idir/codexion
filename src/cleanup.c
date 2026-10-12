/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 12:10:06 by aixel             #+#    #+#             */
/*   Updated: 2026/10/08 16:27:02 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

void	cleandongle(t_dongle *dongle)
{
	if (dongle)
	{
		if (dongle->mutex)
			pthread_mutex_destroy(dongle->mutex);
		free(dongle->mutex);
		free(dongle->heap);
		free(dongle);
	}
}

void	cleancoders(t_coder **coders, int count)
{
	if (!coders)
		return ;
	while (count > 0)
	{
		count--;
		free(coders[count]);
	}
	free(coders);
}

void	cleandongles(t_dongle **dongles, int count)
{
	if (!dongles)
		return ;
	while (count > 0)
	{
		count--;
		cleandongle(dongles[count]);
	}
	free(dongles);
}

void	cleancnf(t_cnf *conf)
{
	if (!conf)
		return ;
	if (conf->coders)
		cleancoders(conf->coders, conf->args[N_CODERS]);
	if (conf->dongles)
		cleandongles(conf->dongles, conf->args[N_CODERS]);
	if (conf->radio)
		pthread_mutex_destroy(conf->radio);
	free(conf->radio);
	if (conf->simmutex)
		pthread_mutex_destroy(conf->simmutex);
	free(conf->simmutex);
	if (conf->changed)
		pthread_cond_destroy(conf->changed);
	free(conf->changed);
	free(conf->args);
	free(conf->start_time);
	free(conf);
}
