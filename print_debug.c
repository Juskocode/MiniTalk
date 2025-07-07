/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_debug.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adavid-a <adavid-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 03:02:42 by adavid-a          #+#    #+#             */
/*   Updated: 2025/07/07 03:02:46 by adavid-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

void	print_pending_signals(void)
{
	sigset_t	pending;
	int			i;

	if (sigpending(&pending) == -1)
	{
		perror("sigpending");
		exit(EXIT_FAILURE);
	}
	printf("\n=== Pending Signals ===\n");
	i = 1;
	while (i < NSIG)
	{
		if (sigismember(&pending, i))
		{
			printf("Signal %d (%s) is pending\n", i, strsignal(i));
		}
		i++;
	}
	printf("=======================\n\n");
}

// Print list of blocked signals
void	print_blocked_signals(void)
{
	sigset_t	blocked;
	int			i;

	if (sigprocmask(SIG_BLOCK, NULL, &blocked) == -1)
	{
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}
	printf("\n=== Blocked Signals ===\n");
	i = 1;
	while (i < NSIG)
	{
		if (sigismember(&blocked, i))
		{
			printf("Signal %d (%s) is blocked\n", i, strsignal(i));
		}
		i++;
	}
	printf("=======================\n");
}
