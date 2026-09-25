/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:52:30 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/26 00:41:34 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

#include <unistd.h> // not sure if used
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>


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
#define ALCERR 1
#define CNDERR 2
#define MTXERR 3
// -----------------
#define FIRST 0
#define SECOND 1

typedef struct s_dongle
{
	int				id;
	int				in_use;
	struct timespec	released_at;
	pthread_mutex_t	mutex;
	pthread_cond_t	ready;
}	t_dongle;


typedef struct s_coder {
	unsigned int	id;
	pthread_t		thread;
	pthread_cond_t	perm;
	pthread_mutex_t	mutex;
	int				*conf;
	t_dongle		*rdongle;
	t_dongle		*ldongle;
	pthread_mutex_t	*talking_pillow;
	int				death_time;
}	t_coder;


typedef struct s_heap {
    int		size;
    int		capacity;
    t_coder	**coders;
} t_heap;

// init
int		init_conf(int arg_cnt, char **args, int *simconf);
t_coder	**init_coders(int *conf);
t_dongle	**init_dongles(int *conf);

// -- debug --
void	display_conf(int *simconf);
void	heap_print(t_heap *heap);

// heap api

t_heap	*heap_init(int capacity);
t_coder *heappop(t_heap *heap);
void    heappush(t_heap *heap, t_coder *newcoder);

#endif
