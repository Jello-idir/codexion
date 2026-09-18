/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/18 11:55:59 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_pthread/_pthread_mutex_t.h>
#include <sys/wait.h>
#include <unistd.h>

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

void say(char who, char *s)
{
	int	i;
	int	len;


	write(1, &who, 1);
	usleep(100000);
	write(1, ": ", 2);
	usleep(100000);

	len = strlen(s);
	i = 0;
	while (i < len)
	{
		write(1, s + i, 1);
		usleep(100000);
		i++;
	}
}

void	*coder_job(void	*arg)
{
	t_coder *coder;

	coder = (t_coder *)arg;
	while (1)
	{
		pthread_mutex_lock(coder->rdongle->mutex);
		pthread_mutex_lock(coder->ldongle->mutex);
		say(coder->id + '@', "is starting ... ");
		usleep(500000);
		say(coder->id + '@', "is DONE\n");
		fflush(stdout);
		pthread_mutex_unlock(coder->rdongle->mutex);
		pthread_mutex_unlock(coder->ldongle->mutex);
		sleep(1);
	}
	return NULL;
}

pthread_t	*start_coders(t_coder **coders, int *simconf)
{
	int	i;
	pthread_t	*coders_thread;

	coders_thread = malloc(sizeof(pthread_t) * simconf[N_CODERS]);
	i = 0;
	while (i < simconf[N_CODERS])
	{
		if (pthread_create((pthread_t *)coders_thread + i, NULL, coder_job, coders[i]))
			return NULL;
		i++;
	}
	return coders_thread;
}

void	add_dongles_to_coders(t_coder **coders, t_dongle **dongles, int *simconf)
{
	int	i;

	i = 0;
	while (i < simconf[N_CODERS])
	{
		printf("adding dongles to %i\n", coders[i]->id);
		coders[i]->ldongle = dongles[i];
		coders[i]->rdongle = dongles[(i + 1) % (simconf[N_CODERS])];
		i++;
	}
}

int main(int ac, char *av[])
{
	int	simconf[8];
	t_coder		**coders;
	t_dongle	**dongles;
	pthread_t *coders_thread_ids;

	if (init_simconf(ac - 1, av + 1, simconf))
		fprintf(stderr, "Error\n");

	fflush(stdout);
	display_simconf(simconf);

	fflush(stdout);
	coders = init_coders(simconf);
	if (!coders)
		fprintf(stderr, "Error\n");

	fflush(stdout);
	dongles = init_dongles(simconf);
	if (!dongles)
		fprintf(stderr, "Error\n");

	add_dongles_to_coders(coders, dongles, simconf);

	fflush(stdout);
	coders_thread_ids = start_coders(coders, simconf);
	if (!coders_thread_ids)
		fprintf(stderr, "Error\n");

	fflush(stdout);

	pthread_join(coders_thread_ids[0], NULL);
	return 0;
}
