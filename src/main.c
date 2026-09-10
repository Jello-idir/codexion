/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/10 17:57:12 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#include <bits/pthreadtypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>


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

void	*coder_job(void *data)
{
	int i;

	t_coder_data *coder_data = (t_coder_data *)(data);

	unsigned int id = coder_data->id;
	char *name = coder_data->name;
	pthread_cond_t *cond = coder_data->cond;
	pthread_mutex_t *mutex = coder_data->mutex;

	while (1)
	{
		printf("%i waiting...\n", id);
		pthread_cond_wait(cond, mutex);

		printf("%u - i\n", id);
		fflush(stdout);
		usleep(200000);

		printf("%u - hate\n", id);
		fflush(stdout);
		usleep(200000);

		printf("%u - this\n", id);
		fflush(stdout);
		usleep(200000);

		printf("\n");
		fflush(stdout);

		usleep(200000);
	}
	return NULL;
}


int main(int ac, char *av[])
{
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	pthread_t	coder[2];


	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	t_coder_data coder_data_0;
	t_coder_data coder_data_1;

	coder_data_0.id = 0;
	coder_data_1.id = 1;

	coder_data_0.mutex = &mutex;
	coder_data_1.mutex = &mutex;

	coder_data_0.cond = &cond;
	coder_data_1.cond = &cond;

	pthread_create(&coder[0], NULL, coder_job, (void *)&coder_data_0);
	pthread_create(&coder[1], NULL, coder_job, (void *)&coder_data_1);

	sleep(2);
	pthread_cond_signal(&cond);

	pthread_cond_broadcast(&cond);

	pthread_join(coder[0], NULL);
	pthread_join(coder[1], NULL);
	return 0;
}
