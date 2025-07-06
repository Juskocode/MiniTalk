# ifndef MINITALK_H
#define MINITALK_H

# include <stdio.h>
# include <errno.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <limits.h>
# include <stdbool.h>

#define TALKING 0
#define QUIET 1

void    ft_signal(int sig, void *handler, bool use_siginfo);
void    ft_kill(pid_t pid, int signum);

#endif

