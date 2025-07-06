# include "minitalk.h"

/* Global flag for synchronization between client and server */
volatile sig_atomic_t	g_state = TALKING;

/**
 * end_handler - Handles server's completion signal
 * @sig: Signal number (unused)
 */
static void	end_handler(int sig)
{
	fputs("\n\t✅ Message received ✅\n", stdout);
	exit(EXIT_SUCCESS);
}

/**
 * ack_handler - Handles server's acknowledgment signals
 * @sig: Signal number (unused)
 */
static void	ack_handler(int sig)
{
	g_state = QUIET;
}

/**
 * send_char - Sends a character bit by bit to server
 * @c: Character to send
 * @kingKai: Server's process ID
 */
static void	send_char(char c, pid_t p)
{
	int	bit;

	bit = 0;
	while (bit < CHAR_BIT)
	{

		/* Send SIGUSR1 for 1, SIGUSR2 for 0 */
		if (c & (0x80 >> bit))
			ft_kill(p, SIGUSR1);
		else
			ft_kill(p, SIGUSR2);
		bit++;

		/* Wait for server acknowledgment */
        while (g_state == TALKING)
			usleep(42);

		// 🧊
		g_state = TALKING;
	}
}

/**
 * main - Client entry point
 * @ac: Argument count (must be 3)
 * @av: Arguments (PID and message)
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int	main(int ac, char **av)
{
	pid_t	p;
	char	*message;
	int		i;

	if (ac != 3)
	{
		fputs("Usage: ./client <kingKai> \"message\"\n", stderr);
		return (EXIT_FAILURE);
	}
	p = atoi(av[1]);
	message = av[2];

	/* Set up signal handlers without siginfo */
	ft_signal(SIGUSR1, ack_handler, false);
	ft_signal(SIGUSR2, end_handler, false);

	i = 0;
	while (message[i])
		send_char(message[i++], p);
	send_char('\0', p);

	return (EXIT_SUCCESS);
}
