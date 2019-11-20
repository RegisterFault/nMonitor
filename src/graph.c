#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "cpuinfo.h"


struct node {
        struct node * next;
        long int foo;
};

struct node * last_elem(struct node * in)
{
        for(in; in->next != NULL; in = in->next) {}
        return in;
}

void add_node(struct node * in, long int a)
{
        in = last_elem(in);
        in->next = malloc(sizeof(struct node));
        in = in->next;
        in->foo = a;
        in->next = NULL;
}

int count_elems(struct node * in)
{
        int i = 0;
        for(in; in->next != NULL; in = in->next)
                i++;
        return i;
}

struct node * nth_elem(struct node * in, int num)
{
        if(num > (count_elems(in) - 1))
                return last_elem(in);
        for(int i = 0; i<num; i++)
                in = in->next;
        return in;
}

struct node * free_top(struct node * in)
{
        struct node * prev;
        
        prev = in;
        in = in->next;
        free(prev);
        
        return in;
}

struct node * free_to_nth(struct node * in, int n)
{
        struct node * prev;
        if(n > count_elems(in))
                return in;
        for(int i=0; i<n; i++) {
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
        return (int) (c-((a/b)*c)) -1;
}

struct node * draw_graph(WINDOW * win, struct node * list, long int max)
{
        int lines = 0;
        int cols = 0;
        int x = 0;
        int y;
        struct node * cur;
        int size = count_elems(list);
        getmaxyx(win,lines,cols);
        wclear(win);
        box(win,0,0);

        while(size > (cols-2) && size > 0 ){
                list = free_top(list);
                size = count_elems(list);
        }
        cur = list;
        
        for(cur; cur->next != NULL; cur = cur->next){
                for(y = get_val_y(max,lines-2,cur->foo); y<lines-2; y++)
                        mvwprintw(win, y+1, x+1, "*");
                x++;
        }
        
        wrefresh(win);
        return list;
}

struct node * draw_wattage(WINDOW *win, struct node * list)
{       
        list = draw_graph(win, list, 35000);
        mvwprintw(win,0,0,"%lld mW -- Battery: %d%%",last_elem(list)->foo,get_bat_pct());
        wrefresh(win);
        add_node(list,get_wattage());
        return list;
}



struct node * draw_freq(WINDOW *win, struct node * list)
{
        list = draw_graph(win, list, 5000);
        mvwprintw(win,0,0,"%lld MHz",last_elem(list)->foo);
        wrefresh(win);
        add_node(list, get_freq());
        return list;
}

void draw_cpu(WINDOW *win)
{
        char *cpu_name = malloc(20);
        struct meminfo mem;
        get_cpuname(&cpu_name, 20);
        get_mem(&mem);

        box(win,0,0);
        mvwprintw(win,0,0,"CPU INFO");
        mvwprintw(win,1,1,cpu_name);
        mvwprintw(win,2,1,"Total: %5ld MB",mem.total/1000);
        mvwprintw(win,3,1,"Avail: %5ld MB",mem.avail/1000);
        mvwprintw(win,4,1,"Free:  %5ld MB",mem.free/1000);
        mvwprintw(win,5,1,"Cache: %5ld MB",mem.cache/1000);
        mvwprintw(win,6,1,"Used:  %5ld MB",(mem.total - mem.avail)/1000);
        wrefresh(win);
}


int main()
{
        WINDOW * wwin;
        WINDOW * fwin;
        WINDOW *cpuwin;
        struct node * wlist = malloc(sizeof(struct node));
        wlist->next = NULL;
        wlist->foo = 0;
        struct node * flist = malloc(sizeof(struct node));
        flist->next = NULL;
        flist->foo = 0;

        initscr();
        curs_set(0);

        cpuwin = newwin(12,20,1,0);
        
        mvprintw(0,20,"Power");
        wwin = newwin(10,50,1,20);
        mvprintw(11,20,"Frequency");
        fwin = newwin(10,50,12,20);
        

        while(1){
                draw_cpu(cpuwin);
                wlist = draw_wattage(wwin, wlist);
                flist = draw_freq(fwin, flist);
                
                refresh();
                usleep(1000000L);
        }

        endwin();

        return 0;
}

