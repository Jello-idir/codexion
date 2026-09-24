/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/23 16:31:19 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#define DELAY 10000

void ssay(char *s) {
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

void	*coder_job(void	*arg)
{
	t_coder *coder;
	int		color;

	coder = (t_coder *)arg;
	color = coder->id;
	while (1) {
		pthread_mutex_lock(coder->rdongle->mutex);
		pthread_mutex_lock(coder->ldongle->mutex);

		pthread_mutex_lock(coder->talking_pillow);
		printf("\033[38;5;%im", color);
		nsay(coder->id);
		ssay(" is working with: ");
		nsay(coder->ldongle->id);
		ssay(" - ");
		nsay(coder->rdongle->id);
		printf("\033[0m\n");
		pthread_mutex_unlock(coder->talking_pillow);

		pthread_mutex_lock(coder->talking_pillow);
		ssay("    ");
		printf("\033[38;5;%im", color);
		nsay(coder->id);
		ssay(" is done\n");
		printf("\033[0m");
		pthread_mutex_unlock(coder->talking_pillow);

		pthread_mutex_unlock(coder->rdongle->mutex);
		pthread_mutex_unlock(coder->ldongle->mutex);

		usleep(coder->conf[DEBUG_T]);
		usleep(coder->conf[REFACTOR_T]);
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

	display_conf(conf);

	return 0;

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
