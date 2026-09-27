/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:52:30 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/26 20:49:57 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

#include "../../../jello/lib/mytools.h"
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
#define D_COOLDOWN 6
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
	int				available;
	struct timespec	released_at;
	pthread_mutex_t	mutex;
	pthread_cond_t	ready;
}	t_dongle;


typedef struct s_coder {
	unsigned int	id;
	pthread_t		thread;
	pthread_cond_t	perm;
	pthread_mutex_t	mutex;
	int				*simconf;
	struct timespec	*start_time;
	t_dongle		*rdongle;
	t_dongle		*ldongle;
	pthread_mutex_t	*radio;
	int				death_time;
}	t_coder;


typedef struct s_heap {
    int		size;
    int		capacity;
    t_coder	**coders;
} t_heap;


typedef struct	s_conf
{
	int				*simconf;
	struct timespec	*start_time;
	t_coder			**coders;
	t_dongle		**dongles;
	pthread_t		*thread_ids;
	pthread_mutex_t	radio;
	t_heap			heap;
}					t_conf;

void print_timestamp(struct timespec *start_time);

// init
int		init_conf(int arg_cnt, char **args, t_conf *conf);
t_coder	**init_coders(t_conf *conf);
t_dongle	**init_dongles(t_conf *conf);

// -- debug --
void	display_conf(int *simconf);
void	heap_print(t_heap *heap);
void	ssay(char *s);
void	nsay(int n);
void	anounce_coder_is_working(t_coder *coder);
void	anounce_coder_is_done(t_coder *coder);

// heap api

t_heap	*heap_init(int capacity);
t_coder *heappop(t_heap *heap);
void    heappush(t_heap *heap, t_coder *newcoder);

#endif
