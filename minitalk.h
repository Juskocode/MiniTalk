/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minitalk.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adavid-a <adavid-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 03:02:24 by adavid-a          #+#    #+#             */
/*   Updated: 2025/07/07 03:02:27 by adavid-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINITALK_H
# define MINITALK_H

# include <errno.h>
# include <limits.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <time.h>
# include "queue.h"
# include "Libft/Libft/libft.h"

# define TALKING 0
# define QUIET 1
/*
** framing parameters
*/
# define MSB                0x80      /* data MSB mask */
# define FRAME_DATA_BITS    8
# define FRAME_PARITY_BITS  4
# define FRAME_BITS         (FRAME_DATA_BITS + FRAME_PARITY_BITS)
# define DELAY_US           120

/*
** sliding window size
*/
# define WINDOW_SIZE        4

/*
** signals for bits
*/
# define BIT1_SIG           SIGUSR1
# define BIT0_SIG           SIGUSR2

/*
** ARQ control signals: ACK, NACK, END
*/
#ifdef SIGRTMIN
# define ACK_SIG            SIGRTMIN
# define NACK_SIG           (SIGRTMIN + 1)
# define END_SIG            (SIGRTMIN + 2)
#else
# define ACK_SIG            SIGALRM
# define NACK_SIG           SIGTERM
# define END_SIG            SIGHUP
#endif

/*
** client states
*/
# define STATE_WAITING      0
# define STATE_ACKED        1
# define STATE_NACKED       2

int	ft_signal(int sig, void *handler, bool use_siginfo);
int	ft_kill(pid_t pid, int signum);

#endif
