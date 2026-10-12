/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aixel <aixel@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:51:29 by aait-idi          #+#    #+#             */
/*   Updated: 2026/10/09 10:51:01 by aixel            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/codexion.h"

int	main(int ac, char *av[])
{
	t_cnf		*cnf;
	pthread_t	*threads;
	int			failed;

	cnf = init_conf(ac - 1, av + 1);
	if (!cnf)
		return (fprintf(stderr, "Error\n"), 1);
	threads = malloc(sizeof(pthread_t) * cnf->args[N_CODERS]);
	if (!threads)
		return (cleancnf(cnf), fprintf(stderr, "Error\n"), 1);
	failed = run_simulation(cnf, threads);
	free(threads);
	cleancnf(cnf);
	if (failed)
		return (fprintf(stderr, "Error\n"), 1);
	return (0);
}
