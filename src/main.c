/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/19 13:18:05 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_pthread/_pthread_mutex_t.h>
#include <sys/wait.h>
#include <unistd.h>

#define DELAY 10000

void ssay(char *s) {
	for (int i = 0; i < (int)strlen(s); i++) {
		write(1, s + i, 1); usleep(DELAY);
	}
}

void nsay(int n) {
	char c = n + '0'; write(1, &c, 1); usleep(DELAY);
}

void	*coder_job(void	*arg)
{
	t_coder *coder;
	int		color;
	char	name[2];

	coder = (t_coder *)arg;
	color = coder->id + 30;
	name[0] = coder->id + '@';
	name[1] = 0;
	while (1)
	{
		pthread_mutex_lock(coder->rdongle->mutex);
		pthread_mutex_lock(coder->ldongle->mutex);

		pthread_mutex_lock(coder->talking_pillow);
		printf("\033[%im", color);
		ssay(name);
		ssay(" is working with: ");
		nsay(coder->ldongle->id);
		ssay(" - ");
		nsay(coder->rdongle->id);
		printf("\033[0m\n");
		pthread_mutex_unlock(coder->talking_pillow);

		sleep(5);


		pthread_mutex_lock(coder->talking_pillow);
		ssay("    ");
		printf("\033[%im", color);
		ssay(name);
		ssay(" is done\n");
		printf("\033[0m");
		pthread_mutex_unlock(coder->talking_pillow);

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
		coders[i]->ldongle = dongles[i];
		coders[i]->rdongle = dongles[(i + 1) % (simconf[N_CODERS])];
		i++;
	}
}

void	add_talking_pillow_to_coders(t_coder **coders, pthread_mutex_t *talking_pillow, int *simconf)
{
	int	i;

	i = 0;
	pthread_mutex_init(talking_pillow, NULL);
	while (i < simconf[N_CODERS])
		coders[i++]->talking_pillow = talking_pillow;
}

int main(int ac, char *av[])
{
	int	simconf[8];
	t_coder				**coders;
	t_dongle			**dongles;
	pthread_t			*coders_thread_ids;
	pthread_mutex_t		talking_pillow;

	setbuf(stdout, NULL);

	if (init_simconf(ac - 1, av + 1, simconf))
		fprintf(stderr, "Error\n");

	display_simconf(simconf);

	coders = init_coders(simconf);
	if (!coders)
		fprintf(stderr, "Error\n");

	dongles = init_dongles(simconf);
	if (!dongles)
		fprintf(stderr, "Error\n");

	add_dongles_to_coders(coders, dongles, simconf);

	add_talking_pillow_to_coders(coders, &talking_pillow, simconf);

	coders_thread_ids = start_coders(coders, simconf);
	if (!coders_thread_ids)
		fprintf(stderr, "Error\n");

	pthread_join(coders_thread_ids[0], NULL);
	return 0;
}
