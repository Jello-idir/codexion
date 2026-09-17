/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:52:30 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/17 22:23:04 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

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

typedef struct s_coder_data {
	unsigned int	id;
	int				*simconf;
}	t_coder;


// -- debug --
void display_simconf(int *simconf);

#endif
