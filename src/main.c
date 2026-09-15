/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/11 11:29:51 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"
#include <stdio.h>

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
		simconf[SCHEDULER] = FIFO;
	else if (strcmp(args[7], "edf") == 0)
		simconf[SCHEDULER] = EDF;
	else
		return 1;
	return 0;
}

int main(int ac, char *av[])
{
	int	simconf[8];

	if (init_simconf(ac - 1, av + 1, simconf))
		fprintf(stderr, "Error\n");
	display_simconf(simconf);

	return 0;
}
