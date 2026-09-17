/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/17 22:52:08 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#include <pthread.h>
#include <stdio.h>
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

t_coder	**init_coders_data(int *simconf)
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

void	*coder_job(void	*coder_data)
{
	int counter;

	counter = 0;
	while (1) {
		sleep(1);
		printf("%c ", ((t_coder *)(coder_data))->id + 'A');
		printf("-> %i\n", counter);
		counter++;
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

int main(int ac, char *av[])
{
	int	simconf[8];
	t_coder	**coders;
	pthread_t *coders_thread_ids;

	if (init_simconf(ac - 1, av + 1, simconf))
		fprintf(stderr, "Error\n");
	display_simconf(simconf);
	coders = init_coders_data(simconf);
	if (!coders)
		fprintf(stderr, "Error\n");
	coders_thread_ids = start_coders(coders, simconf);
	pthread_join(coders_thread_ids[0], NULL);
	return 0;
}
