#include <ncurses.h>
#include <stdlib.h>
#include "list.h"


struct node *init_node(void)
{
        struct node *out = malloc(sizeof(struct node));
        out->next = NULL;
        out->data = 0;
        return out;
}

struct node *last_elem(struct node *in)
{
        for( in; in->next != NULL; in = in->next) {}
        return in;
}

void add_node(struct node *in, long int data)
{
        struct node *curs = last_elem(in);
        curs->next = malloc(sizeof(struct node));
        curs = curs->next;
        curs->data = data;
        curs->next = NULL;
}

int count_elems(struct node *in)
{
        int i = 0;
        for (in; in->next != NULL; in = in->next)
                i++;
        return i;
}

struct node *nth_elem(struct node *in, int num)
{
        if (num > (count_elems(in) - 1))
                return last_elem(in);
        for (int i = 0; i<num; i++)
                in = in->next;
        return in;
}

struct node *free_top(struct node *in)
{
        struct node *prev;
        
        prev = in;
        in = in->next;
        free(prev);
        
        return in;
}

struct node *free_to_nth(struct node *in, int n)
{
        struct node *prev;
        if (n > count_elems(in))
                return in;

        for (int i=0; i<n; i++) {
                prev = in;
                in = in->next;
                free(prev);
        }
        return in;
}

void trunc_list(struct node **list, int limit)
{
        int size = count_elems(*list);
        
        while (size > limit && size > 0 ){
                *list = free_top(*list);
                size--;
        }
}


