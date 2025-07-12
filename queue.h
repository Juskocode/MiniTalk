/* queue.h */
#ifndef QUEUE_H
# define QUEUE_H

# include <stdlib.h>
# include <stdio.h>
# include "Libft/Libft/libft.h"

typedef struct s_queue
{
    t_list  *head;
}               t_queue;

t_queue     *queue_init(void);
void        queue_push_back(t_queue *q, void *content);
void        *queue_front(t_queue *q);
void        *queue_pop_front(t_queue *q);
int         queue_empty(t_queue *q);
int         queue_size(t_queue *q);
void        queue_clear(t_queue *q, void (*del)(void *));

#endif

