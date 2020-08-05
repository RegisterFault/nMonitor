#define _GNU_SOURCE
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include "cpuinfo.h"
#include "list.h"
#include "msr.h"
#include "draw.h"

struct {
        WINDOW *wwin;
        WINDOW *fwin;
        WINDOW *cpuwin;
        WINDOW *memwin;
        WINDOW *fgwin;
        struct node *wlist;
        struct node *flist;
#define STATS_MODE 0
#define GRID_MODE 1
        int mode;
} app;
        

void draw_app()
{
        /* ncurses setup */
        initscr();
        curs_set(0);
        noecho();
        cbreak();
        nodelay(stdscr, 1);
        
        app.cpuwin = newwin(15, 20, 0, 0);
        app.memwin = newwin(9, 20, 15, 0);
        app.wwin =   newwin(12, 50, 0, 20);
        app.fwin =   newwin(12, 50, 12, 20);
        app.fgwin =  newwin(24,70,0,0);
}

void init_app()
{
        init_batinfo();
        init_powerinfo();

        app.wlist = init_node();
        app.flist = init_node();
        app.mode = STATS_MODE;

        draw_app();
}

void handle_resize(int unused)
{
        endwin();
        draw_app();
}

int main()
{

        init_app();
        signal(SIGWINCH, handle_resize);

        while (1) {
                if(getch() == 'f')
                        app.mode = !app.mode;
                flushinp();

                if (app.mode == STATS_MODE) {
                        draw_cpu(app.cpuwin);
                        draw_mem(app.memwin);
                        draw_power(app.wwin, &app.wlist);
                        draw_freq(app.fwin, &app.flist);
                        doupdate();
                } else if (app.mode == GRID_MODE) {
                        draw_grid(app.fgwin);
                        doupdate();
                }

                usleep(DUR);
        }

        endwin();

        return 0;
}

