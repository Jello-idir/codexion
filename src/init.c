/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 10:51:01 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

pthread_cond_t	*init_cond(void)
{
	pthread_cond_t	*cond;

	cond = malloc(sizeof(pthread_cond_t));
	if (!cond)
		return (NULL);
	if (pthread_cond_init(cond, NULL))
		return (free(cond), NULL);
	return (cond);
}

pthread_mutex_t	*init_mutex(void)
{
	pthread_mutex_t	*mutex;

	mutex = malloc(sizeof(pthread_mutex_t));
	if (!mutex)
		return (NULL);
	if (pthread_mutex_init(mutex, NULL))
		return (free(mutex), NULL);
	return (mutex);
}

static int	other_init(t_cnf *conf)
{
	conf->start_time = malloc(sizeof(struct timespec));
	if (!conf->start_time)
		return (1);
	if (clock_gettime(CLOCK_MONOTONIC, conf->start_time) == -1)
		return (1);
	conf->radio = init_mutex();
	if (!conf->radio)
		return (1);
	conf->simmutex = init_mutex();
	if (!conf->simmutex)
		return (1);
	conf->changed = init_cond();
	if (!conf->changed)
		return (1);
	return (0);
}

static void	add_dongles_to_coders(t_cnf *c)
{
	int	i;

	i = 0;
	while (i < c->args[N_CODERS])
	{
		c->coders[i]->ldongle = c->dongles[i];
		c->coders[i]->rdongle = c->dongles[(i + 1) % (c->args[N_CODERS])];
		i++;
	}
}

t_cnf	*init_conf(int arg_cnt, char **args)
{
	t_cnf	*conf;

	conf = malloc(sizeof(t_cnf));
	if (!conf)
		return (NULL);
	memset(conf, 0, sizeof(t_cnf));
	conf->args = init_simconf(arg_cnt, args);
	if (!conf->args)
		return (cleancnf(conf), NULL);
	if (other_init(conf))
		return (cleancnf(conf), NULL);
	conf->coders = init_coders(conf);
	if (!conf->coders)
		return (cleancnf(conf), NULL);
	conf->dongles = init_dongles(conf);
	if (!conf->dongles)
		return (cleancnf(conf), NULL);
	conf->stopped = 0;
	add_dongles_to_coders(conf);
	return (conf);
}
