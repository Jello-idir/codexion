/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 23:12:20 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

static int	parse_number(const char *arg)
{
	int	value;
	int	digit;

	value = 0;
	if (*arg == '+')
		arg++;
	if (!*arg)
		return (-1);
	while (*arg)
	{
		if (*arg < '0' || *arg > '9')
			return (-1);
		digit = *arg++ - '0';
		if (value > (INT_MAX - digit) / 10)
			return (-1);
		value = value * 10 + digit;
	}
	return (value);
}

int	*init_simconf(int arg_cnt, char **args)
{
	int	*simconf;
	int	i;

	if (arg_cnt != 8)
		return (NULL);
	if (strcmp(args[7], "fifo") && strcmp(args[7], "edf"))
		return (NULL);
	simconf = malloc(sizeof(int) * 8);
	if (!simconf)
		return (NULL);
	simconf[SCHEDULER] = FIFO;
	if (strcmp(args[7], "edf") == 0)
		simconf[SCHEDULER] = EDF;
	i = 0;
	while (i < 7)
	{
		simconf[i] = parse_number(args[i]);
		if (simconf[i] < 0 || (!simconf[i] && i != D_COOLDOWN))
			return (free(simconf), NULL);
		i++;
	}
	return (simconf);
}
