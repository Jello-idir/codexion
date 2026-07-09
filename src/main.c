/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-idi <aait-idi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/07/09 14:44:46 by aait-idi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#include <stdio.h>
#include <string.h>

int init_simconf(int arg_cnt, char **args, int *simconf)
{
	int i;

	if (arg_cnt != 8)
		return 1;
	i = 0;
	while (i < 7)
	{
		simconf[i] = atoi(args[i]);
		if (simconf[i] <= 0)
			return 1;
		i++;
	}
	if (strcmp(args[7], "fifo") == 0)
		simconf[7] = FIFO;
	else if (strcmp(args[7], "edf") == 0)
		simconf[7] = EDF;
	else
		return 1;
	return 0;
}


int main(int ac, char *av[])
{
	int simconf[8];

	if (init_simconf(ac - 1, av + 1, simconf))
		fprintf(stderr, "\033[31mError\033[0m\n");
	display_simconf(simconf);
}
