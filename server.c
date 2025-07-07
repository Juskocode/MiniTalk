/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adavid-a <adavid-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 02:57:55 by adavid-a          #+#    #+#             */
/*   Updated: 2025/07/07 03:06:58 by adavid-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

/**
 * handler - Processes incoming signals to reconstruct characters
 * @sig: Signal type (SIGUSR1 or SIGUSR2)
 * @info: Signal information containing sender's PID
 * @more_info: Additional context (unused)
 * Note: Uses static variables to maintain state between signal calls
 */
static void	handler(int sig, siginfo_t *info, void *more_info)
{
	static char		c = 0;
	static int		bit = 0;
	static pid_t	p = 0;

	if (info->si_pid)
		p = info->si_pid;
	if (sig == SIGUSR1)
		c |= (0b10000000 >> bit);
	else if (sig == SIGUSR2)
		c &= ~(0x80 >> bit);
	bit++;
	if (bit == CHAR_BIT)
	{
		bit = 0;
		if (c == '\0')
		{
			write(STDOUT_FILENO, "\n", 1);
			ft_kill(p, SIGUSR2);
			c = 0;
			return ;
		}
		write(STDOUT_FILENO, &c, 1);
		c = 0;
	}
	ft_kill(p, SIGUSR1);
}

/**
 * main - Server entry point
 * @ac: Argument count (must be 1)
 * @av: Argument vector (unused)
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int	main(int ac, char **av)
{
	int	s1;
	int	s2;

	if (ac != 1)
	{
		fputs("Usage: ./server\n", stderr);
		return (EXIT_FAILURE);
	}
	printf("Server PID: %d\n", getpid());
	s1 = ft_signal(SIGUSR1, handler, true);
	if (s1 == -1)
		perror("Failed to assign signal for SIGUSR1");
	s2 = ft_signal(SIGUSR2, handler, true);
	if (s2 == -1)
		perror("Failed to assign signal for SIGUSR2");
	while (42)
		pause();
	return (EXIT_SUCCESS);
}
