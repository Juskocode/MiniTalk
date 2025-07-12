/* queue.c */
#include "../queue.h"

t_queue *queue_init(void)
{
    t_queue    *q;

    q = malloc(sizeof(*q));
    if (!q)
        return (NULL);
    q->head = NULL;
    return (q);
}

void queue_push_back(t_queue *q, void *content)
{
    t_list  *new;

    if (!q)
        return ;
    new = ft_lstnew(content);
    if (!new)
        return ;
    ft_lstadd_back(&q->head, new);
}

void *queue_front(t_queue *q)
{
    if (!q || !q->head)
        return (NULL);
    return (q->head->content);
}

void *queue_pop_front(t_queue *q)
{
    t_list  *tmp;
    void    *content;

    if (!q || !q->head)
        return (NULL);
    tmp = q->head;
    content = tmp->content;
    q->head = tmp->next;
    free(tmp);
    return (content);
}

int queue_empty(t_queue *q)
{
    return (!q || q->head == NULL);
}

int queue_size(t_queue *q)
{
    return (ft_lstsize(q->head));
}

void queue_clear(t_queue *q, void (*del)(void *))
{
    t_list  *next;

    if (!q)
        return ;
    while (q->head)
    {
        next = q->head->next;
        if (del)
            del(q->head->content);
        free(q->head);
        q->head = next;
    }
    free(q);
}

