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


	/* Store client PID only once per connection */
	if (info->si_pid) 
		p = info->si_pid;

	/* Build character bit by bit (MSB first) */
	if (sig == SIGUSR1)
		c |= (0b10000000 >> bit);
	else if (sig == SIGUSR2)
		c &= ~(0x80 >> bit);
	bit++;

	/* Process complete character */
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

	if (ac != 1)
	{
		fputs("Usage: ./server\n", stderr);
		return (EXIT_FAILURE);
	}
	
	printf("Server PID: %d\n", getpid());

	/* Set up signal handlers with siginfo enabled */
	ft_signal(SIGUSR1, handler, true);
	ft_signal(SIGUSR2, handler, true);

	while (42)
		pause();
		
	return (EXIT_SUCCESS);
}
