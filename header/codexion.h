/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:52:30 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 16:12:44 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <unistd.h>
# include <stdio.h>
# include <limits.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <time.h>

# define N_CODERS 0
# define BURNOUT_T 1
# define COMPILE_T 2
# define DEBUG_T 3
# define REFACTOR_T 4
# define N_COMPILES 5
# define D_COOLDOWN 6
# define SCHEDULER 7

# define FIFO 1
# define EDF 2

typedef struct s_heap	t_heap;
typedef struct s_cnf	t_cnf;

typedef struct s_dongle
{
	int					id;
	int					available;
	pthread_mutex_t		*mutex;
	t_heap				*heap;
	unsigned long long	cooldown;
	struct timespec		released_at;
}						t_dongle;

typedef struct s_coder
{
	unsigned int		id;
	int					ncompile;
	t_cnf				*conf;
	t_dongle			*rdongle;
	t_dongle			*ldongle;
	unsigned long long	death_time;
}						t_coder;

typedef struct s_request
{
	t_coder				*coder;
	unsigned long long	deadline;
}	t_request;

struct s_heap
{
	int			size;
	int			policy;
	t_request	requests[2];
};

typedef struct s_cnf
{
	int				*args;
	t_coder			**coders;
	t_dongle		**dongles;
	struct timespec	*start_time;
	pthread_mutex_t	*radio;
	pthread_mutex_t	*simmutex;
	pthread_cond_t	*changed;
	int				queued;
	int				stopped;
}					t_cnf;

pthread_mutex_t		*init_mutex(void);
t_cnf				*init_conf(int arg_cnt, char **args);
t_coder				**init_coders(t_cnf *conf);
t_dongle			**init_dongles(t_cnf *conf);
int					*init_simconf(int arg_cnt, char **args);

void				cleandongle(t_dongle *dongle);
void				cleancoders(t_coder **coders, int count);
void				cleandongles(t_dongle **dongles, int count);
void				cleancnf(t_cnf *cnf);

t_heap				*heap_init(int policy);
t_coder				*heappeek(t_heap *heap);
void				heappush(t_heap *heap, t_coder *newcoder);
void				heapremove(t_heap *heap, t_coder *coder);

unsigned long long	ts_to_ns(struct timespec *ts);
unsigned long long	get_simulation_time(struct timespec *start_time);
int					simulation_sleep(t_cnf *conf, int duration);

int					isstopped(t_cnf *conf);
void				log_locked(t_coder *coder, unsigned long long now,
						char *what);
int					radio_message(t_coder *coder, char *what);
int					get_dongles(t_coder *coder);
void				lock_dongles(t_coder *coder);
void				unlock_dongles(t_coder *coder);
int					dongles_available(t_coder *coder);
unsigned long long	dongles_ready_at(t_coder *coder);
void				wait_for_dongles(t_coder *coder);
void				wait_cooldown(t_cnf *conf, unsigned long long end);
void				queue_requests(t_coder *coder);
void				release_dongles(t_coder *coder);
void				wake_dongles(t_cnf *conf);
void				*coderjob(void *arg);
void				*monitor(void *arg);
int					run_simulation(t_cnf *conf, pthread_t *threads);

#endif
