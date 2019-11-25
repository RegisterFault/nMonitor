#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "cpuinfo.h"
#include "list.h"
#include "msr.h"

#define DUR (500000L)
#define DUR_SEC (DUR*0.000001)

struct node * draw_wattage(WINDOW *win, struct node * list)
{       
        list = draw_graph(win, list, 35000);
        mvwprintw(win,0,0,"%lld mW -- Battery: %d%% -- Capacity: %d/%d Wh",
                        last_elem(list)->foo,
                        get_bat_pct(),
                        get_bat_full(),
                        get_bat_design());
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
        int line = 0;
        static double last_pkg_nrg = 0;
        char * governor = get_governor();
        char *cpu_brand;
        
        bzero(cpu_name,20);
        get_cpuname(&cpu_name, 20);
        cpu_brand = is_amd() ? "AMD" : "Intel";

        wclear(win);
        box(win,0,0);

        mvwprintw(win,line++,0,"CPU INFO: %s",cpu_brand);
        mvwprintw(win,line++,1,"%s %dC/%dT",
                        cpu_name,
                        get_cores(),
                        get_threads());
        mvwprintw(win,line++,1,"Temp: %dC",get_temp());
        mvwprintw(win,line++,1,"Boost: %s ",get_turbo() ? "on" : "off" );
        mvwprintw(win,line++,1,"Gov: %s",governor);
        if(geteuid() == 0){ /* if we are root */
                if(last_pkg_nrg != 0)
                        mvwprintw(win,line++,1,"PKG:   %6.2f W",(get_pkg_joules()-last_pkg_nrg)/DUR_SEC);
                else
                        mvwprintw(win,line++,1,"PKG:   %6.2f W",0.0);
                last_pkg_nrg = get_pkg_joules();
                if(!is_amd()){
                        mvwprintw(win, line++, 1, "Throttle: %c", get_throttle_char());
                        mvwprintw(win,line++,1,"CPU:   %+2.2f mV",get_volt(CPU_PLANE));
                        mvwprintw(win,line++,1,"CACHE: %+2.2f mV",get_volt(CACHE_PLANE));
                }

        }
        free(governor);
        wrefresh(win);
}

void draw_mem(WINDOW * win)
{
        struct meminfo mem;
        int line = 0;
        
        get_mem(&mem);
        wclear(win);
        box(win,0,0);

        mvwprintw(win,line++,0,"MEM INFO");
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
        WINDOW *memwin;
        struct node * wlist = malloc(sizeof(struct node));
        wlist->next = NULL;
        wlist->foo = 0;
        struct node * flist = malloc(sizeof(struct node));
        flist->next = NULL;
        flist->foo = 0;

        initscr();
        curs_set(0);

        cpuwin = newwin(14,20,0,0);
        memwin = newwin(10,20,14,0);
        
        wwin = newwin(12,50,0,20);
        fwin = newwin(12,50,12,20);

        /* this helps reduce glitchiness during initial loading */
        refresh();

        while(1){
                draw_cpu(cpuwin);
                draw_mem(memwin);
                wlist = draw_wattage(wwin, wlist);
                flist = draw_freq(fwin, flist);
                
                refresh();
                usleep(DUR);
        }

        endwin();

        return 0;
}

