/* src/server_arq.c */
#include "../minitalk.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

/*
** Per-client receive context
*/
typedef struct s_ctx
{
    pid_t            pid;
    uint16_t         buf;
    int              idx;      // bits accumulated
    char            *msg;
    size_t           len, cap;
    struct s_ctx    *next;
}               t_ctx;

static t_ctx    *g_clients = NULL;
static int       g_logfd;

/* Fast popcount helper */
static int popcount(unsigned char c)
{
    int cnt = 0;
    while (c)
    {
        cnt += (c & 1);
        c >>= 1;
    }
    return cnt;
}

/* Find or create a t_ctx for this pid */
static t_ctx *get_ctx(pid_t pid)
{
    t_ctx **p = &g_clients;
    while (*p)
    {
        if ((*p)->pid == pid)
            return *p;
        p = &(*p)->next;
    }
    t_ctx *c = malloc(sizeof(*c));
    if (!c) exit(EXIT_FAILURE);
    c->pid = pid;
    c->buf = 0;
    c->idx = 0;
    c->msg = NULL;
    c->len = c->cap = 0;
    c->next = g_clients;
    g_clients = c;
    return c;
}

/* Remove and free the context for pid */
static void drop_ctx(pid_t pid)
{
    t_ctx **p = &g_clients;
    while (*p)
    {
        if ((*p)->pid == pid)
        {
            t_ctx *t = *p;
            *p = t->next;
            free(t->msg);
            free(t);
            return;
        }
        p = &(*p)->next;
    }
}

/*
** The SIGUSR1/SIGUSR2 handler.  We:
**  1) Identify the client by info->si_pid
**  2) Shift one bit into its buf
**  3) When we have FRAME_BITS, extract the byte + 4 parity bits
**  4) Parity check → ACK / NACK back to that pid
**  5) If byte == 0, that’s end-of-message: log & print it, send END_SIG,
**     then free that client’s context so we can start fresh for the next msg.
*/
static void handler(int sig, siginfo_t *info, void *u)
{
    (void)u;
    pid_t    pid = info->si_pid;
    if (pid == 0)
        return;

    t_ctx *c = get_ctx(pid);

    /* shift in new bit */
    c->buf = (c->buf << 1) | (sig == BIT1_SIG);
    c->idx++;

    if (c->idx == FRAME_BITS)
    {
        unsigned char byte = (c->buf >> FRAME_PARITY_BITS) & 0xFF;
        unsigned int  got  = c->buf & ((1 << FRAME_PARITY_BITS) - 1);

        if (byte == '\0')
        {
            /* end of this client’s message */
            dprintf(g_logfd, "\n-- End of message from %d --\n\n", pid);

            /* Print once on stdout */
            write(STDOUT_FILENO, "Full msg from ", 14);
            dprintf(STDOUT_FILENO, "%d: \"%.*s\"\n\n",
                    pid, (int)c->len, c->msg);

            ft_kill(pid, END_SIG);
            drop_ctx(pid);
        }
        else
        {
            int real = popcount(byte);
            if ((unsigned)real == got)
            {
                /* store and ACK */
                if (c->len + 1 >= c->cap)
                {
                    c->cap = c->cap ? c->cap * 2 : 128;
                    c->msg = realloc(c->msg, c->cap);
                }
                c->msg[c->len++] = byte;

                dprintf(g_logfd, "ACK  %d '%c'\n", pid, byte);
                ft_kill(pid, ACK_SIG);
            }
            else
            {
                dprintf(g_logfd,
                    "NACK %d '%c' (got %u, need %d)\n",
                    pid, byte, got, real);
                ft_kill(pid, NACK_SIG);
            }
        }

        /* reset for next frame */
        c->buf = 0;
        c->idx = 0;
    }
}

int main(void)
{
    /* 1) Open (append) the server log */
    g_logfd = open("42server.log",
                   O_CREAT|O_WRONLY|O_APPEND,
                   S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (g_logfd < 0)
        perror("open 42server.log"), exit(EXIT_FAILURE);

    /* 2) Pretty‐print PID box */
    {
        char buf[64];
        int  w = snprintf(buf, sizeof(buf), " Server PID: %d ", getpid());
        putchar('\n');
        putchar('+'); for (int i = 0; i < w; i++) putchar('-'); puts("+");
        putchar('|'); fputs(buf, stdout); puts("|");
        putchar('+'); for (int i = 0; i < w; i++) putchar('-'); puts("+\n");
    }

    /* 3) Log run header */
    dprintf(g_logfd, "===== New run @ %lld =====\n",
            (long long)time(NULL));

    /* 4) Install bit‐handlers */
    ft_signal(BIT1_SIG, handler, true);
    ft_signal(BIT0_SIG, handler, true);

    /* 5) Loop forever—every client can start anytime */
    while (1)
        pause();

    /* never reached */
    close(g_logfd);
    return EXIT_SUCCESS;
}

