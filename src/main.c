/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/09 17:51:58 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#include <bits/pthreadtypes.h>
#include <stdio.h>
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

void *coder_a_job(void *arg)
{
	void *mtx = ((void **)arg)[0];
	int *value = ((int **)arg)[1];

	pthread_mutex_lock(mtx);
	printf("\nCODER A:\n");
	printf("coder-a locked the mutex\n");

	printf("coder-a got value: %d\n", *value);

	usleep(1000000);
	*value += 1;

	pthread_mutex_unlock(mtx);
	printf("coder-a unlocked the mutex\n");
	return NULL;
}

void *coder_b_job(void *arg)
{
	void *mtx = ((void **)arg)[0];
	int *value = ((int **)arg)[1];

	pthread_mutex_lock(mtx);
	printf("\nCODER B:\n");
	printf("coder-b locked the mutex\n");

	printf("coder-b got value: %d\n", *value);

	usleep(100000);
	*value += 1;

	pthread_mutex_unlock(mtx);
	printf("coder-b unlocked the mutex\n");
	return NULL;
}

int main(int ac, char *av[])
{
	setvbuf(stdout, NULL, _IONBF, 0);
	int simconf[8];
	pthread_t coder_a;
	pthread_t coder_b;

	printf("codexion!\n");
	if (init_simconf(ac - 1, av + 1, simconf))
		fprintf(stderr, "Error\n");

	pthread_mutex_t fastmutex = PTHREAD_MUTEX_INITIALIZER;
	int val = 13;
	void *mutex_and_value[2] = {&fastmutex, &val};

	pthread_mutex_lock(&fastmutex);

	if (pthread_create(&coder_a, NULL, coder_a_job, mutex_and_value))
		fprintf(stderr, "Error\n");

	if (pthread_create(&coder_b, NULL, coder_b_job, mutex_and_value))
		fprintf(stderr, "Error\n");

	sleep(1);
	pthread_mutex_unlock(&fastmutex);

	pthread_join(coder_a, NULL);
	pthread_join(coder_b, NULL);


	printf("Final value: %d\n", val);

	// display_simconf(simconf);
}
