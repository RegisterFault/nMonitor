#define _GNU_SOURCE
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
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
        WINDOW *fgwin;
        struct node *wlist = init_node();
        struct node *flist = init_node();
#define STATS_MODE 0
#define GRID_MODE 1
        int mode = STATS_MODE;

        init_batinfo();
        init_powerinfo();

        /* ncurses setup */
        initscr();
        curs_set(0);
        noecho();
        cbreak();
        nodelay(stdscr, 1);

        cpuwin = newwin(14, 20, 0, 0);
        memwin = newwin(10, 20, 14, 0);
        wwin = newwin(12, 50, 0, 20);
        fwin = newwin(12, 50, 12, 20);
        fgwin = newwin(24,70,0,0);

        while (1) {
                if (mode == STATS_MODE) {
                        draw_cpu(cpuwin);
                        draw_mem(memwin);
                        draw_power(wwin, &wlist);
                        draw_freq(fwin, &flist);
                        doupdate();
                } else if (mode == GRID_MODE) {
                        draw_grid(fgwin);
                        doupdate();
                }
                
                if(getch() == 'f')
                        mode = !mode;
                flushinp();

                usleep(DUR);
        }

        endwin();

        return 0;
}

