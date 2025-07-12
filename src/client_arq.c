/* src/client_arq.c */
#include "../minitalk.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct s_frame
{
    uint8_t c;
    int     seq;
    int     status;
}               t_frame;

/* state for ACK/NACK */
volatile sig_atomic_t g_state = STATE_WAITING;
/* state for END_SIG */
volatile sig_atomic_t g_end   = 0;

/* ARQ control handlers */
static void ack_handler(int sig)   { (void)sig; g_state = STATE_ACKED;  }
static void nack_handler(int sig)  { (void)sig; g_state = STATE_NACKED; }
static void end_handler(int sig)   { (void)sig; g_end = 1; }

/*
** send one 12-bit frame (8 data bits + 4 pop-count bits)
*/
static void send_frame(uint8_t c, pid_t pid)
{
    int            i;
    unsigned int   pc = 0;

    /* data bits */
    for (i = 0; i < FRAME_DATA_BITS; i++)
    {
        if (c & (MSB >> i)) ft_kill(pid, BIT1_SIG);
        else                ft_kill(pid, BIT0_SIG);
        usleep(DELAY_US);
        pc += (c >> i) & 1;
    }
    /* parity bits */
    for (i = FRAME_PARITY_BITS - 1; i >= 0; i--)
    {
        if (pc & (1u << i)) ft_kill(pid, BIT1_SIG);
        else                ft_kill(pid, BIT0_SIG);
        usleep(DELAY_US);
    }
}

/*
** send one frame and block until exactly one ACK or NACK arrives
*/
static int send_and_wait(uint8_t c, pid_t pid)
{
    g_state = STATE_WAITING;
    send_frame(c, pid);
    while (g_state == STATE_WAITING)
        pause();
    return g_state;
}

/*
** send the end‐of‐message marker and wait for END_SIG
*/
static void send_end(pid_t pid)
{
    g_end = 0;
    /* we send a frame of 12 zero‐bits */
    send_frame((uint8_t)'\0', pid);
    while (!g_end)
        pause();
}

/*
** Log the current window
*/
static void log_window(int fd, int window_id, t_frame *win, int sz)
{
    dprintf(fd, "Window %d - \"", window_id);
    for (int i = 0; i < sz; i++)
        dprintf(fd, "%c", win[i].c);
    dprintf(fd, "\" Size %d (", sz);
    for (int i = 0; i < sz; i++)
    {
        dprintf(fd, "%d:%s",
            win[i].seq,
            win[i].status == STATE_ACKED ? "ACK" : "NACK");
        if (i + 1 < sz) dprintf(fd, ", ");
    }
    dprintf(fd, ")\n");
}

int main(int ac, char **av)
{
    pid_t    pid;
    char    *msg;
    int      data_len, next;
    int      client_fd;
    int      seq = 0, win_id = 0;
    t_frame  window[WINDOW_SIZE];
    int      win_sz, i;

    if (ac != 3)
    {
        printf("Usage: %s <PID> \"message\"\n", av[0]);
        return EXIT_FAILURE;
    }
    pid     = (pid_t)atoi(av[1]);
    msg     = av[2];
    /* data_len is number of real chars, excluding the '\0' terminator */
    data_len = (int)strlen(msg);

    /* open client log */
    client_fd = open("42client.log",
                     O_CREAT|O_WRONLY|O_APPEND,
                     S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (client_fd < 0)
    {
        perror("open 42client.log");
        return EXIT_FAILURE;
    }
    dprintf(client_fd,
        "\n===== New run PID %d @ %lld =====\n",
        getpid(), (long long)time(NULL));

    /* install ARQ control handlers */
    ft_signal(ACK_SIG,   ack_handler,  false);
    ft_signal(NACK_SIG,  nack_handler, false);
    ft_signal(END_SIG,   end_handler,  false);

    /* 1) Pipeline up to WINDOW_SIZE data‐frames */
    next  = 0;
    win_sz = 0;
    while (next < data_len && win_sz < WINDOW_SIZE)
    {
        uint8_t c = (uint8_t)msg[next++];
        window[win_sz].c      = c;
        window[win_sz].seq    = ++seq;
        window[win_sz].status = send_and_wait(c, pid);
        win_sz++;
    }
    log_window(client_fd, ++win_id, window, win_sz);

    /* 2) Go–Back–N ARQ on the data‐frames */
    while (win_sz > 0)
    {
        if (window[0].status == STATE_ACKED)
        {
            /* slide out acknowledged frame */
            for (i = 1; i < win_sz; i++)
                window[i - 1] = window[i];
            win_sz--;

            /* enqueue next data char, if any */
            if (next < data_len)
            {
                uint8_t c = (uint8_t)msg[next++];
                window[win_sz].c      = c;
                window[win_sz].seq    = ++seq;
                window[win_sz].status = send_and_wait(c, pid);
                win_sz++;
            }
        }
        else /* NACK: retransmit entire window */
        {
            for (i = 0; i < win_sz; i++)
                window[i].status = send_and_wait(window[i].c, pid);
        }
        log_window(client_fd, ++win_id, window, win_sz);
    }

    /* 3) finally send the terminator and wait for END_SIG */
    send_end(pid);

    printf("Done ✅\n");
    close(client_fd);
    return EXIT_SUCCESS;
}

