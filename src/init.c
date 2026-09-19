/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:26 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/18 12:43:48 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

int init_simconf(int arg_cnt, char **args, int *simconf)
{
	int i;

	if (arg_cnt != 8)
		return 1;
	i = 0;
	while (i < 7)
	{
		simconf[i] = atoi(args[i]);
		if (simconf[i] <= 0)
			return 1;
		i++;
	}
	if (strcmp(args[7], "fifo") == 0)
		simconf[SCHEDULER] = FIFO;
	else if (strcmp(args[7], "edf") == 0)
		simconf[SCHEDULER] = EDF;
	else
		return 1;
	return 0;
}

t_coder	**init_coders(int *simconf)
{
	int		i;
	t_coder	**coders;

	coders = malloc(sizeof(t_coder *) * simconf[N_CODERS]);
	if (!coders)
		return NULL;
	i = 0;
	while (i < simconf[N_CODERS])
	{
		coders[i] = malloc(sizeof(t_coder));
		if (!coders[i])
			return NULL;
		coders[i]->id = i + 1;
		coders[i]->simconf = simconf;
		i++;
	}
	return coders;
}

t_dongle	**init_dongles(int *simconf)
{
	int	i;
	t_dongle **dongles;

	dongles = malloc(sizeof(t_dongle *) * simconf[N_CODERS]);
	if (!dongles)
		return NULL;
	i = 0;
	while (i < simconf[N_CODERS])
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
