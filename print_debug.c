# include "minitalk.h"

void    print_pending_signals()
{
        sigset_t pending;
        if (sigpending(&pending) == -1) {
            perror("sigpending");
            exit(EXIT_FAILURE);
        }

        printf("\n=== Pending Signals ===\n");
        for (int i = 1; i < NSIG; i++) {
            if (sigismember(&pending, i)) {
                printf("Signal %d (%s) is pending\n", i, strsignal(i));
            }
        }
        printf("=======================\n\n");
}

// Print list of blocked signals
void    print_blocked_signals()
{
        sigset_t    blocked;
        if (sigprocmask(SIG_BLOCK, NULL, &blocked) == -1) {
            perror("sigprocmask");
            exit(EXIT_FAILURE);
        }
        printf("\n=== Blocked Signals ===\n");
        for (int i = 1; i < NSIG; i++) {
            if (sigismember(&blocked, i)) {
                printf("Signal %d (%s) is blocked\n", i, strsignal(i));
            }
        }
        printf("=======================\n");
}
