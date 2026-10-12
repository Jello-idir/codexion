/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 14:34:02 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

static t_coder	*init_coder(t_cnf *conf, int index)
{
	t_coder	*coder;

	coder = malloc(sizeof(t_coder));
	if (!coder)
		return (NULL);
	memset(coder, 0, sizeof(t_coder));
	coder->mutex = init_mutex();
	if (!coder->mutex)
		return (free(coder), NULL);
	coder->id = index + 1;
	coder->ncompile = conf->args[N_COMPILES];
	coder->conf = conf;
	coder->death_time = conf->args[BURNOUT_T];
	coder->ldongle = NULL;
	coder->rdongle = NULL;
	return (coder);
}

t_coder	**init_coders(t_cnf *conf)
{
	t_coder	**coders;
	int		i;

	coders = malloc(sizeof(t_coder *) * conf->args[N_CODERS]);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < conf->args[N_CODERS])
	{
		coders[i] = init_coder(conf, i);
		if (!coders[i])
			return (cleancoders(coders, i), NULL);
		i++;
	}
	return (coders);
}

static t_dongle	*init_dongle(t_cnf *conf, int index)
{
	t_dongle	*dongle;

	dongle = malloc(sizeof(t_dongle));
	if (!dongle)
		return (NULL);
	memset(dongle, 0, sizeof(t_dongle));
	dongle->mutex = init_mutex();
	if (!dongle->mutex)
		return (cleandongle(dongle), NULL);
	dongle->heap = heap_init(conf->args[SCHEDULER]);
	if (!dongle->heap)
		return (cleandongle(dongle), NULL);
	dongle->id = index + 1;
	dongle->available = 1;
	dongle->cooldown = conf->args[D_COOLDOWN];
	dongle->released_at.tv_sec = 0;
	dongle->released_at.tv_nsec = 0;
	return (dongle);
}

t_dongle	**init_dongles(t_cnf *conf)
{
	t_dongle	**dongles;
	int			i;

	dongles = malloc(sizeof(t_dongle *) * conf->args[N_CODERS]);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < conf->args[N_CODERS])
	{
		dongles[i] = init_dongle(conf, i);
		if (!dongles[i])
			return (cleandongles(dongles, i), NULL);
		i++;
	}
	return (dongles);
}
