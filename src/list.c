#include <ncurses.h>
#include <stdlib.h>
#include "list.h"


struct node *init_node(void)
{
        /* add_node should be the only thing allocating */
        return NULL;
}

struct node *last_elem(struct node *in)
{
        if(in == NULL)
                return NULL;
        for( in; in->next != NULL; in = in->next) {}
        return in;
}

void add_node(struct node **in, long int data)
{
        struct node *curs;
        if(*in == NULL){
                *in = malloc(sizeof(struct node));
                curs = *in;
        } else {
                curs = last_elem(*in);
                curs->next = malloc(sizeof(struct node));
                curs = curs->next;
        }
        curs->data = data;
        curs->next = NULL;
}

int count_elems(struct node *in)
{
        int i = 1;
        if(in == NULL)
                return 0;
        for (in; in->next != NULL; in = in->next)
                i++;
        return i;
}

struct node *nth_elem(struct node *in, int index)
{
        if (index > count_elems(in))
                return last_elem(in);

        for (int i = 0; i < index; i++)
                in = in->next;
        return in;
}

struct node *free_top(struct node *in)
{
        struct node *prev;
        if(in == NULL)
                return in;
        
        prev = in;
        in = in->next;
        free(prev);
        
        return in;
}

void free_to_nth(struct node **in, int n)
{
        struct node *prev;
        if(*in == NULL)
                return;

        if (n > count_elems(*in))
                return;

        for (int i = 0; i < n; i++) {
                prev = *in;
                *in = (*in)->next;
                free(prev);
        }
}

void free_list(struct node *in)
{
        struct node *prev;
        do{
                prev = in;
                in = in->next;
                free(prev);
        } while(in != NULL);       
}

void trunc_list(struct node **list, int limit)
{
        int size = count_elems(*list);
        
        while (size > limit && size > 0 ){
                *list = free_top(*list);
                size--;
        }
}


