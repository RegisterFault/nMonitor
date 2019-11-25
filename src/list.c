#include <ncurses.h>
#include <stdlib.h>
#include "list.h"



struct node *last_elem(struct node *in)
{
        for( in; in->next != NULL; in = in->next) {}
        return in;
}

void add_node(struct node *in, long int a)
{
        in = last_elem(in);
        in->next = malloc(sizeof(struct node));
        in = in->next;
        in->foo = a;
        in->next = NULL;
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

int get_val_y(long int max, long int lines, long int in)
{
        float a = in;
        float b = max;
        float c = lines;
        return (int) (c - ((a / b) * c)) -1;
}

struct node *draw_graph(WINDOW *win, struct node *list, long int max)
{
        int lines = 0;
        int cols = 0;
        int x = 0;
        int y;
        struct node *cur;
        int size = count_elems(list);
        getmaxyx(win, lines, cols);
        wclear(win);
        box(win, 0, 0);

        while (size > (cols - 2) && size > 0 ){
                list = free_top(list);
                size = count_elems(list);
        }
        cur = list;
        
        for(cur; cur->next != NULL; cur = cur->next){
                for(y = get_val_y(max, lines - 2, cur->foo); y < lines - 2; y++)
                        mvwprintw(win, y + 1, x + 1, "*");
                x++;
        }
        
        return list;
}

