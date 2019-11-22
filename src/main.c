#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "cpuinfo.h"
#include "list.h"

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
        int line = 0;
        
        bzero(cpu_name,20);
        get_cpuname(&cpu_name, 20);
        get_mem(&mem);

        box(win,0,0);
        mvwprintw(win,line++,0,"CPU INFO");
        mvwprintw(win,line++,1,"%s %dC/%dT",
                        cpu_name,
                        get_cores(),
                        get_threads());
        mvwprintw(win,line++,1,"Temp: %dC",get_temp());
        mvwprintw(win,line++,1,"Turbo: %s ",get_turbo() ? "on" : "off" );
        if(geteuid() == 0){
                mvwprintw(win, line++, 1, "Throttle: %c", get_throttle_char());
                mvwprintw(win, line++, 1, "PL1: %d PL2: %d", get_pl1(), get_pl2());
        }
        mvwprintw(win,line++,1,"Total: %5ld MB",mem.total/1000);
        mvwprintw(win,line++,1,"Avail: %5ld MB",mem.avail/1000);
        mvwprintw(win,line++,1,"Free:  %5ld MB",mem.free/1000);
        mvwprintw(win,line++,1,"Cache: %5ld MB",mem.cache/1000);
        mvwprintw(win,line++,1,"Used:  %5ld MB",(mem.total - mem.avail)/1000);
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
        wwin = newwin(11,50,1,20);
        mvprintw(12,20,"Frequency");
        fwin = newwin(11,50,13,20);
        

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

