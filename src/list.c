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


int calc_y(long int max, long int lines, long int in)
{
        float a = in;
        float b = max;
        float c = lines;
        return (int) (c - ((a / b) * c)) -1;
}

void draw_points(WINDOW *win, struct node **list, int max, char point)
{
        int lines = 0;
        int cols = 0;
        int x,y;
        struct node *cur;

        getmaxyx(win, lines, cols);
        lines -= 2; //account for box
        
        for(x = 0, cur = *list; cur->next != NULL; cur = cur->next){
                for(y = calc_y(max, lines, cur->data); y < lines; y++)
                        mvwprintw(win, y + 1, x + 1, "%c", point);
                x++;
        }
}

void draw_graph(WINDOW *win, struct node **list, long int max)
{
        int lines = 0;
        int cols = 0;
        
        getmaxyx(win, lines, cols);
        wclear(win);
        box(win, 0, 0);
        
        trunc_list(list, cols - 2);
        draw_points(win, list, max, '*'); 
}

