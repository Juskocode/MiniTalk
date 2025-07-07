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

# define TALKING 0
# define QUIET 1
# define MSB 0x80 // 128 -> 0b10000000

int	ft_signal(int sig, void *handler, bool use_siginfo);
int	ft_kill(pid_t pid, int signum);

#endif
