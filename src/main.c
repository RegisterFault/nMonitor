#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "cpuinfo.h"
#include "list.h"
#include "msr.h"
#include "draw.h"

int main()
{
        WINDOW *wwin;
        WINDOW *fwin;
        WINDOW *cpuwin;
        WINDOW *memwin;
        struct node *wlist = init_node();
        struct node *flist = init_node();

        init_batinfo();

        /* ncurses setup */
        initscr();
        curs_set(0);

        cpuwin = newwin(14, 20, 0, 0);
        memwin = newwin(10, 20, 14, 0);
        wwin = newwin(12, 50, 0, 20);
        fwin = newwin(12, 50, 12, 20);

        while (1) {
                draw_cpu(cpuwin);
                draw_mem(memwin);
                draw_power(wwin, &wlist);
                draw_freq(fwin, &flist);
                
                doupdate();
                usleep(DUR);
        }

        endwin();

        return 0;
}

