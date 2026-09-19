/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:52:30 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/19 10:33:54 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

#include <sys/_pthread/_pthread_mutex_t.h>
#include <sys/_pthread/_pthread_t.h>
#include <unistd.h> // not sure if used
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// number_of_coders
// time_to_burnout
// time_to_compile
// time_to_debug
// time_to_refactor
// number_of_compiles_required
// dongle_cooldown
// scheduler

// -----------------
#define N_CODERS 0
#define BURNOUT_T 1
#define COMPILE_T 2
#define DEBUG_T 3
#define REFACTOR_T 4
#define N_COMPILES 5
#define D_COODLDOWN 6
#define SCHEDULER 7
// -----------------
#define FIFO 1
#define EDF 2
// -----------------


typedef struct s_dongle {
	unsigned int	id;
	pthread_mutex_t	*mutex;
}	t_dongle;

typedef struct s_coder {
	unsigned int	id;
	pthread_t		thread;
	int				*simconf;
	t_dongle		*rdongle;
	t_dongle		*ldongle;
	pthread_mutex_t	*talking_pillow;
}	t_coder;

// init
int		init_simconf(int arg_cnt, char **args, int *simconf);
t_coder	**init_coders(int *simconf);
t_dongle	**init_dongles(int *simconf);

// -- debug --
void	display_simconf(int *simconf);

#endif
