/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/29 23:31:53 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

unsigned long long	ts_to_ms(struct timespec *ts)
{
    return (ts->tv_sec * 1000 + ts->tv_nsec / 1000000);
}

unsigned long long get_simulation_time(struct timespec *start_time)
{
	struct timespec now;
	now.tv_nsec = 0;
	now.tv_sec = 0;

	clock_gettime(CLOCK_MONOTONIC, &now);
	return (ts_to_ms(&now) - ts_to_ms(start_time));
}

void	dongle_cooldown(t_dongle *dongle)
{
	struct timespec now;

	pthread_mutex_lock(&dongle->mutex);
	usleep(500);
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

unsigned long long	radio_message(t_coder *coder, char *what)
{
	unsigned long long now;

	pthread_mutex_lock(coder->radio);
	now = get_simulation_time(coder->start_time);
	printf("%05llu", now);
	printf("\033[38;5;%dm", coder->id % 7 + 1);
	printf(" %d %s\n", coder->id, what);
	printf("\033[0m");
	pthread_mutex_unlock(coder->radio);
	return (now);
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
	unsigned long long compile_start;

	get_dongles(coder);
	compile_start = radio_message(coder, "is compiling");
	coder->death_time = compile_start + coder->simconf[BURNOUT_T];
	usleep(coder->simconf[COMPILE_T] * 1000);
	release_dongles(coder);
}

void debug(t_coder *coder)
{
	radio_message(coder, "is debugging");
	usleep(coder->simconf[DEBUG_T] * 1000);
}

void refactor(t_coder *coder)
{
	radio_message(coder, "is refactoring");
	usleep(coder->simconf[REFACTOR_T] * 1000);
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

pthread_t	*start_coders(t_conf *conf)
{
	int	i;
	pthread_t	*coders_thread;

	coders_thread = malloc(sizeof(pthread_t) * conf->simconf[N_CODERS]);
	i = 0;
	while (i < conf->simconf[N_CODERS])
	{
		if (pthread_create((pthread_t *)coders_thread + i, NULL, coder_job, conf->coders[i]))
			return NULL;
		i++;
	}
	return coders_thread;
}

void	add_dongles_to_coders(t_conf *conf)
{
	int	i;

	i = 0;
	while (i < conf->simconf[N_CODERS])
	{
		conf->coders[i]->ldongle = conf->dongles[i];
		conf->coders[i]->rdongle = conf->dongles[(i + 1) % (conf->simconf[N_CODERS])];
		i++;
	}
}

void	add_talking_pillow_to_coders(t_conf *conf)
{
	int	i;

	i = 0;
	while (i < conf->simconf[N_CODERS])
		conf->coders[i++]->radio = &conf->radio;
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

	add_dongles_to_coders(&conf);
	add_talking_pillow_to_coders(&conf);

	conf.thread_ids = start_coders(&conf);
	if (!conf.thread_ids)
		fprintf(stderr, "Error\n");

	pthread_join(conf.thread_ids[0], NULL);
	return 0;
}
