/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:26 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/23 15:51:41 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

int init_conf(int arg_cnt, char **args, int *conf)
{
	int i;

	if (arg_cnt != 8)
		return 1;
	i = 0;
	while (i < 7)
	{
		conf[i] = atoi(args[i]);
		if (conf[i] <= 0)
			return 1;
		i++;
	}
	if (strcmp(args[7], "fifo") == 0)
		conf[SCHEDULER] = FIFO;
	else if (strcmp(args[7], "edf") == 0)
		conf[SCHEDULER] = EDF;
	else
		return 1;
	return 0;
}

t_coder	**init_coders(int *conf)
{
	int		i;
	t_coder	**coders;

	coders = malloc(sizeof(t_coder *) * conf[N_CODERS]);
	if (!coders)
		return NULL;
	i = 0;
	while (i < conf[N_CODERS])
	{
		coders[i] = malloc(sizeof(t_coder));
		if (!coders[i])
			return NULL;
		coders[i]->id = i + 1;
		coders[i]->conf = conf;
		i++;
	}
	return coders;
}

t_dongle	**init_dongles(int *conf)
{
	int	i;
	t_dongle **dongles;

	dongles = malloc(sizeof(t_dongle *) * conf[N_CODERS]);
	if (!dongles)
		return NULL;
	i = 0;
	while (i < conf[N_CODERS])
	{
		dongles[i] = malloc(sizeof(t_dongle));
		if (!dongles[i])
			return NULL;
		dongles[i]->id = i +1;
		dongles[i]->mutex = malloc(sizeof(pthread_mutex_t));
		if (!dongles[i]->mutex)
			return NULL;
		if (pthread_mutex_init(dongles[i]->mutex, NULL))
			return NULL;
		i++;
	}
	return dongles;
}
