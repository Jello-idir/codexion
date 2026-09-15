/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 13:24:02 by aait-idi          #+#    #+#             */
/*   Updated: 2026/09/11 10:04:30 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

void display_simconf(int *simconf)
{
	char *txt[] = {
		"number_of_coders",
		"time_to_burnout",
		"time_to_compile",
		"time_to_debug",
		"time_to_refactor",
		"number_of_compiles",
		"dongle_cooldown",
		"scheduler",
	};
	printf("------------------------------\n");
	printf("\033[2m");
	for (int i = 0; i < 7; i++)
		printf("- %-20s : %i\n", txt[i], simconf[i]);
	char *type = "unknown";
	if (simconf[7] == FIFO)
		type = "fifo";
	else if (simconf[7] == EDF)
		type = "edf";
	printf("- %-20s : %s\n", txt[7], type);
	printf("\033[0m");
	printf("------------------------------\n");
}
