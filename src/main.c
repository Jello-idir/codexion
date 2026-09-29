/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/28 00:52:10 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/_pthread/_pthread_mutex_t.h>
#include <time.h>
#include <unistd.h>

void	dongle_cooldown(t_dongle *dongle)
{
	struct timespec now;

	pthread_mutex_lock(&dongle->mutex);
	usleep(5000000);
	dongle->available = 1;
	pthread_cond_signal(&dongle->ready);
	pthread_mutex_unlock(&dongle->mutex);
}
void	get_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	while(!dongle->available)
		pthread_cond_wait(&dongle->ready, &dongle->mutex);
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongle(t_dongle *dongle)
{
	dongle_cooldown(dongle);
}

void	radio_message(t_coder *coder, char *what)
{
	pthread_mutex_lock(coder->radio);
	print_timestamp(coder->start_time);
	printf("\033[1;3;38;5;%dm", coder->id % 7 + 1);
	printf(" %d %s\n", coder->id, what);
	printf("\033[0m");
	pthread_mutex_unlock(coder->radio);
}

void	get_dongles(t_coder *coder)
{
	t_dongle *dongle[2];

	dongle[FIRST] = coder->ldongle;
	dongle[SECOND] = coder->rdongle;

	if (coder->rdongle->id % 2)
	{
		dongle[FIRST] = coder->rdongle;
		dongle[SECOND] = coder->ldongle;
	}
	get_dongle(dongle[FIRST]);
	radio_message(coder, "has taken a dongle");
	get_dongle(dongle[SECOND]);
	radio_message(coder, "has taken a dongle");
}

void	release_dongles(t_coder *coder)
{
	release_dongle(coder->ldongle);
	release_dongle(coder->rdongle);
}

void	compile(t_coder *coder)
{
	get_dongles(coder);
	radio_message(coder, "is compiling");
	usleep(coder->simconf[COMPILE_T] * 1000);
	release_dongles(coder);
}

void debug(t_coder *coder)
{
	radio_message(coder, "is debugging");
	usleep(coder->simconf[DEBUG_T]);
}

void refactor(t_coder *coder)
{
	radio_message(coder, "is refactoring");
	usleep(coder->simconf[REFACTOR_T]);
}

void	*coder_job(void	*arg)
{
	t_coder *coder;

	coder = (t_coder *)arg;
	while (1)
	{
		compile(coder);
		debug(coder);
		refactor(coder);
	}
	return NULL;
}

pthread_t	*start_coders(t_conf conf)
{
	int	i;
	pthread_t	*coders_thread;

	coders_thread = malloc(sizeof(pthread_t) * conf.simconf[N_CODERS]);
	i = 0;
	while (i < conf.simconf[N_CODERS])
	{
		if (pthread_create((pthread_t *)coders_thread + i, NULL, coder_job, conf.coders[i]))
			return NULL;
		i++;
	}
	return coders_thread;
}

void	add_dongles_to_coders(t_conf conf)
{
	int	i;

	i = 0;
	while (i < conf.simconf[N_CODERS])
	{
		conf.coders[i]->ldongle = conf.dongles[i];
		conf.coders[i]->rdongle = conf.dongles[(i + 1) % (conf.simconf[N_CODERS])];
		i++;
	}
}

void	add_talking_pillow_to_coders(t_conf conf)
{
	int	i;

	i = 0;
	pthread_mutex_init(&conf.radio, NULL);
	while (i < conf.simconf[N_CODERS])
		conf.coders[i++]->radio = &conf.radio;
}

int main(int ac, char *av[])
{
	t_conf	conf;
	setbuf(stdout, NULL);

	if (init_conf(ac - 1, av + 1, &conf))
		fprintf(stderr, "Error\n");

	clock_gettime(CLOCK_MONOTONIC, conf.start_time);

	conf.coders = init_coders(&conf);
	if (!conf.coders)
		fprintf(stderr, "Errors\n");

	conf.dongles = init_dongles(&conf);
	if (!conf.dongles)
		fprintf(stderr, "Error\n");

	add_dongles_to_coders(conf);
	add_talking_pillow_to_coders(conf);

	conf.thread_ids = start_coders(conf);
	if (!conf.thread_ids)
		fprintf(stderr, "Error\n");

	pthread_join(conf.thread_ids[0], NULL);
	return 0;
}
