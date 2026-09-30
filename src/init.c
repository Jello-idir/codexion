/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:26 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/30 22:39:42 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#include <limits.h>

static void	cleanup_coders(t_coder **coders, int count)
{
	while (count > 0)
	{
		--count;
		pthread_mutex_destroy(&coders[count]->mutex);
		pthread_cond_destroy(&coders[count]->perm);
		free(coders[count]);
	}
	free(coders);
}

static void	cleanup_dongles(t_dongle **dongles, int count)
{
	while (count > 0)
	{
		--count;
		pthread_mutex_destroy(&dongles[count]->mutex);
		pthread_cond_destroy(&dongles[count]->ready);
		free(dongles[count]);
	}
	free(dongles);
}

static t_coder	*init_coder(t_conf *conf, int index)
{
	t_coder	*coder;

	coder = malloc(sizeof(*coder));
	if (!coder)
		return (NULL);
	memset(coder, 0, sizeof(*coder));
	if (pthread_cond_init(&coder->perm, NULL))
		return (free(coder), NULL);
	if (pthread_mutex_init(&coder->mutex, NULL))
	{
		pthread_cond_destroy(&coder->perm);
		return (free(coder), NULL);
	}
	coder->id = index + 1;
	coder->simconf = conf->simconf;
	coder->start_time = conf->start_time;
	coder->radio = conf->radio;
	coder->ldongle = NULL;
	coder->rdongle = NULL;
	coder->death_time = conf->simconf[BURNOUT_T];
	return (coder);
}

static t_coder	**init_coders(t_conf *conf)
{
	t_coder	**coders;
	int		i;

	coders = malloc(sizeof(*coders) * conf->simconf[N_CODERS]);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < conf->simconf[N_CODERS])
	{
		coders[i] = init_coder(conf, i);
		if (!coders[i])
			return (cleanup_coders(coders, i), NULL);
		i++;
	}
	return (coders);
}

static t_dongle	*init_dongle(int index)
{
	t_dongle	*dongle;

	dongle = malloc(sizeof(*dongle));
	if (!dongle)
		return (NULL);
	if (pthread_cond_init(&dongle->ready, NULL))
		return (free(dongle), NULL);
	if (pthread_mutex_init(&dongle->mutex, NULL))
	{
		pthread_cond_destroy(&dongle->ready);
		return (free(dongle), NULL);
	}
	dongle->id = index + 1;
	dongle->available = 1;
	dongle->released_at.tv_sec = 0;
	dongle->released_at.tv_nsec = 0;
	return (dongle);
}

static t_dongle	**init_dongles(t_conf *conf)
{
	t_dongle	**dongles;
	int			i;

	dongles = malloc(sizeof(*dongles) * conf->simconf[N_CODERS]);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < conf->simconf[N_CODERS])
	{
		dongles[i] = init_dongle(i);
		if (!dongles[i])
			return (cleanup_dongles(dongles, i), NULL);
		i++;
	}
	return (dongles);
}

static int	parse_positive(const char *arg)
{
	int	value;
	int	digit;

	value = 0;
	if (*arg == '+')
		arg++;
	if (!*arg)
		return (0);
	while (*arg)
	{
		if (*arg < '0' || *arg > '9')
			return (0);
		digit = *arg++ - '0';
		if (value > (INT_MAX - digit) / 10)
			return (0);
		value = value * 10 + digit;
	}
	return (value);
}

static int	*init_simconf(int arg_cnt, char **args)
{
	int	*simconf;
	int	i;

	if (arg_cnt != 8)
		return (NULL);
	if (strcmp(args[7], "fifo") && strcmp(args[7], "edf"))
		return (NULL);
	simconf = malloc(sizeof(*simconf) * 8);
	if (!simconf)
		return (NULL);
	simconf[SCHEDULER] = FIFO;
	if (strcmp(args[7], "edf") == 0)
		simconf[SCHEDULER] = EDF;
	i = 0;
	while (i < 7)
	{
		simconf[i] = parse_positive(args[i]);
		if (!simconf[i])
			return (free(simconf), NULL);
		i++;
	}
	return (simconf);
}

static int	other_init(t_conf *conf)
{
	conf->start_time = malloc(sizeof(*conf->start_time));
	if (!conf->start_time)
		return (1);
	if (clock_gettime(CLOCK_MONOTONIC, conf->start_time) == -1)
		return (free(conf->start_time), 1);
	conf->radio = malloc(sizeof(*conf->radio));
	if (!conf->radio)
		return (free(conf->start_time), 1);
	if (pthread_mutex_init(conf->radio, NULL))
		return (free(conf->start_time), free(conf->radio), 1);
	return (0);
}

static void	cleanup_conf(t_conf *conf)
{
	if (conf->coders)
		cleanup_coders(conf->coders, conf->simconf[N_CODERS]);
	free(conf->simconf);
	free(conf->start_time);
	pthread_mutex_destroy(conf->radio);
	free(conf->radio);
	free(conf);
}

t_conf	*init_conf(int arg_cnt, char **args)
{
	t_conf	*conf;

	conf = malloc(sizeof(*conf));
	if (!conf)
		return (NULL);
	conf->simconf = init_simconf(arg_cnt, args);
	if (!conf->simconf)
		return (free(conf), NULL);
	if (other_init(conf))
		return (free(conf->simconf), free(conf), NULL);
	conf->coders = init_coders(conf);
	if (!conf->coders)
		return (cleanup_conf(conf), NULL);
	conf->dongles = init_dongles(conf);
	if (!conf->dongles)
		return (cleanup_conf(conf), NULL);
	return (conf);
}
