/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:26 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/26 20:59:22 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#include <pthread.h>

void	cleanup_coders(t_coder **coders, size_t index, int	error)
{
	if (!coders)
		return;
	if (error >= MTXERR)
		pthread_mutex_destroy(&coders[index]->mutex);
	if (error >= CNDERR)
		pthread_cond_destroy(&coders[index]->perm);
	if (error >= ALCERR)
		free(coders[index]);
	if (index > 0)
		index--;
	while (index <= 0)
	{
		if (coders[index])
		{
			pthread_mutex_destroy(&coders[index]->mutex);
			pthread_cond_destroy(&coders[index]->perm);
		}
		free(coders[index]);
		index--;
	}
}

void	cleanup_dongles(t_dongle **dongles, size_t index, int error)
{
	if (!dongles)
		return;
	if (error >= MTXERR)
		pthread_mutex_destroy(&dongles[index]->mutex);
	if (error >= CNDERR)
		pthread_cond_destroy(&dongles[index]->ready);
	if (error >= ALCERR)
		free(dongles[index]);
	if (index > 0)
		index--;
	while (index <= 0)
	{
		if (dongles[index])
		{
			pthread_mutex_destroy(&dongles[index]->mutex);
			pthread_cond_destroy(&dongles[index]->ready);
		}
		free(dongles[index]);
		index--;
	}
}

int init_conf(int arg_cnt, char **args, t_conf *conf)
{
	int i;

	if (arg_cnt != 8)
		return 1;
	pthread_mutex_init(&conf->radio, NULL);
	conf->simconf = malloc(sizeof(int) * 8);
	if (!conf->simconf)
		return 1;
	conf->start_time = malloc(sizeof(struct timespec));
	if (!conf->start_time)
		return (free(conf->simconf), 1);
	i = 0;
	while (i < 7)
	{
		conf->simconf[i] = atoi(args[i]);
		if (conf->simconf[i] <= 0)
			return 1;
		i++;
	}
	if (strcmp(args[7], "fifo") == 0)
		conf->simconf[SCHEDULER] = FIFO;
	else if (strcmp(args[7], "edf") == 0)
		conf->simconf[SCHEDULER] = EDF;
	else
		return 1;
	return 0;
}

t_coder	**init_coders(t_conf *conf)
{
	ssize_t	i;
	t_coder	**coders;
	int		errcode;

	coders = malloc(sizeof(t_coder *) * conf->simconf[N_CODERS]);
	if (!coders)
		return NULL;
	errcode = 0;
	i = 0;
	while (i < conf->simconf[N_CODERS])
	{
		coders[i] = malloc(sizeof(t_coder));
		if (!coders[i])
			errcode = ALCERR;
		if (pthread_cond_init(&coders[i]->perm, NULL))
			errcode = CNDERR;
		if (pthread_mutex_init(&coders[i]->mutex, NULL))
			errcode = MTXERR;
		if (errcode)
			return (cleanup_coders(coders, i, errcode), free(coders), NULL);
		coders[i]->id = i + 1;
		coders[i]->simconf = conf->simconf;
		coders[i]->start_time = conf->start_time;
		i++;
	}
	return coders;
}

t_dongle	**init_dongles(t_conf *conf)
{
	size_t		i;
	t_dongle	**dongles;
	int			errcode;

	dongles = malloc(sizeof(t_coder *) * conf->simconf[N_CODERS]);
	if (!dongles)
		return NULL;
	errcode = 0;
	i = 0;
	while (i < conf->simconf[N_CODERS])
	{
		dongles[i] = malloc(sizeof(t_coder));
		if (!dongles[i])
			errcode = ALCERR;
		if (pthread_cond_init(&dongles[i]->ready, NULL))
			errcode = CNDERR;
		if (pthread_mutex_init(&dongles[i]->mutex, NULL))
			errcode = MTXERR;
		if (errcode)
			return (cleanup_dongles(dongles, i, errcode), free(dongles), NULL);
		dongles[i]->id = i + 1;
		dongles[i]->available = 1;
		dongles[i]->released_at.tv_sec = 0;
		dongles[i]->released_at.tv_nsec = 0;
		i++;
	}
	return dongles;
}

