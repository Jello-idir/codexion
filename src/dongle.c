/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 14:33:23 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

int	dongles_available(t_coder *coder)
{
	return (coder->ldongle->available && coder->rdongle->available
		&& heappeek(coder->ldongle->heap) == coder
		&& heappeek(coder->rdongle->heap) == coder);
}

/* Callers hold simmutex before locking dongles in ascending ID order. */
void	lock_dongles(t_coder *coder)
{
	if (coder->ldongle->id < coder->rdongle->id)
	{
		pthread_mutex_lock(coder->ldongle->mutex);
		pthread_mutex_lock(coder->rdongle->mutex);
	}
	else
	{
		pthread_mutex_lock(coder->rdongle->mutex);
		if (coder->ldongle != coder->rdongle)
			pthread_mutex_lock(coder->ldongle->mutex);
	}
}

void	unlock_dongles(t_coder *coder)
{
	pthread_mutex_unlock(coder->ldongle->mutex);
	if (coder->ldongle != coder->rdongle)
		pthread_mutex_unlock(coder->rdongle->mutex);
}

void	release_dongles(t_coder *coder)
{
	struct timespec	now;

	pthread_mutex_lock(coder->conf->simmutex);
	lock_dongles(coder);
	clock_gettime(CLOCK_MONOTONIC, &now);
	coder->ldongle->released_at = now;
	coder->rdongle->released_at = now;
	coder->ldongle->available = 1;
	coder->rdongle->available = 1;
	unlock_dongles(coder);
	pthread_cond_broadcast(coder->conf->changed);
	pthread_mutex_unlock(coder->conf->simmutex);
}

void	wake_dongles(t_cnf *cnf)
{
	pthread_mutex_lock(cnf->simmutex);
	pthread_cond_broadcast(cnf->changed);
	pthread_mutex_unlock(cnf->simmutex);
}
