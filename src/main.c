/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/26 00:46:01 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#define DELAY 10000

void ssay(char *s)
{
	for (int i = 0; i < (int)strlen(s); i++) {
		write(1, s + i, 1); usleep(DELAY);
	}
}

void nsay(int n) {
	char	c = n % 10 + '0';
	if (n > 9)
		nsay(n / 10);
	write(1, &c, 1);
	usleep(DELAY);
}

void	anounce_coder_is_working(t_coder *coder)
{
	// locking pillow
	pthread_mutex_lock(coder->talking_pillow);

	printf("\033[38;5;%im", coder->id);
	nsay(coder->id);
	ssay(" is working with: ");
	nsay(coder->ldongle->id);
	ssay(" - ");
	nsay(coder->rdongle->id);
	printf("\033[0m\n");

	//unlocking pillow
	pthread_mutex_unlock(coder->talking_pillow);

}

void	anounce_coder_is_done(t_coder *coder)
{
	// locking pillow
	pthread_mutex_lock(coder->talking_pillow);

	ssay("    ");
	printf("\033[38;5;%im", coder->id);
	nsay(coder->id);
	ssay(" is done\n");
	printf("\033[0m");

	// unlocking pillow
	pthread_mutex_unlock(coder->talking_pillow);
}

void	dongle_cooldown(t_dongle *dongle)
{
	struct timespec now;

	pthread_mutex_lock(&dongle->mutex);
	dongle->in_use = 0;
	pthread_cond_signal(&dongle->ready);
	pthread_mutex_unlock(&dongle->mutex);
}

long	elapsed_us(struct timespec start, struct timespec end)
{
	long	sec;
	long	nsec;

	sec = end.tv_sec - start.tv_sec;
	nsec = end.tv_nsec - start.tv_nsec;
	return (sec * 1000000 + nsec / 1000);
}

void	get_dongle(t_dongle *dongle)
{
	struct timespec	now;
	struct timespec	wait;
	long			elapsed;
	long			remaining;

	pthread_mutex_lock(&dongle->mutex);

	while (1)
	{
		while (dongle->in_use)
			pthread_cond_wait(&dongle->ready, &dongle->mutex);

		clock_gettime(CLOCK_MONOTONIC, &now);

		elapsed = elapsed_us(dongle->released_at, now);
		remaining = 999999 - elapsed;

		if (remaining <= 0)
		{
			dongle->in_use = 1;
			break;
		}

		wait.tv_sec = remaining / 1000000;
		wait.tv_nsec = (remaining % 1000000) * 1000;

		pthread_cond_timedwait_relative_np(
			&dongle->ready,
			&dongle->mutex,
			&wait
		);
	}

	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);

	clock_gettime(CLOCK_MONOTONIC, &dongle->released_at);
	dongle->in_use = 0;

	pthread_cond_broadcast(&dongle->ready);
	pthread_mutex_unlock(&dongle->mutex);
}

void	compile(t_coder *coder)
{
	t_dongle *dongle[2];

	dongle[FIRST] = coder->ldongle;
	dongle[SECOND] = coder->rdongle;

	if (coder->ldongle->id > coder->rdongle->id)
	{
		dongle[FIRST] = coder->rdongle;
		dongle[SECOND] = coder->ldongle;
	}
	get_dongle(dongle[FIRST]);
	get_dongle(dongle[SECOND]);
	anounce_coder_is_working(coder);
	usleep(coder->conf[COMPILE_T]);
	sleep(3);
	release_dongle(dongle[FIRST]);
	release_dongle(dongle[SECOND]);
	anounce_coder_is_done(coder);
}

void debug(t_coder *coder)
{
	usleep(coder->conf[DEBUG_T]);
}

void refactor(t_coder *coder)
{
	usleep(coder->conf[REFACTOR_T]);
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

pthread_t	*start_coders(t_coder **coders, int *conf)
{
	int	i;
	pthread_t	*coders_thread;

	coders_thread = malloc(sizeof(pthread_t) * conf[N_CODERS]);
	i = 0;
	while (i < conf[N_CODERS])
	{
		if (pthread_create((pthread_t *)coders_thread + i, NULL, coder_job, coders[i]))
			return NULL;
		i++;
	}
	return coders_thread;
}

void	add_dongles_to_coders(t_coder **coders, t_dongle **dongles, int *conf)
{
	int	i;

	i = 0;
	while (i < conf[N_CODERS])
	{
		coders[i]->ldongle = dongles[i];
		coders[i]->rdongle = dongles[(i + 1) % (conf[N_CODERS])];
		i++;
	}
}

void	add_talking_pillow_to_coders(t_coder **coders, pthread_mutex_t *talking_pillow, int *conf)
{
	int	i;

	i = 0;
	pthread_mutex_init(talking_pillow, NULL);
	while (i < conf[N_CODERS])
		coders[i++]->talking_pillow = talking_pillow;
}

int main(int ac, char *av[])
{
	int					conf[8];
	t_coder				**coders;
	t_dongle			**dongles;
	pthread_t			*coders_thread_ids;
	pthread_mutex_t		talking_pillow;
	t_heap				heap;
	setbuf(stdout, NULL);

	if (init_conf(ac - 1, av + 1, conf))
		fprintf(stderr, "Error\n");

	coders = init_coders(conf);
	if (!coders)
		fprintf(stderr, "Error\n");

	dongles = init_dongles(conf);
	if (!dongles)
		fprintf(stderr, "Error\n");

	add_dongles_to_coders(coders, dongles, conf);
	add_talking_pillow_to_coders(coders, &talking_pillow, conf);

	coders_thread_ids = start_coders(coders, conf);
	if (!coders_thread_ids)
		fprintf(stderr, "Error\n");

	pthread_join(coders_thread_ids[0], NULL);
	return 0;
}
