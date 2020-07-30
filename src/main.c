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

void draw_HWP(WINDOW *win)
{
        int line = 1;
        HWP_REQUEST foo;

        box(win, 0, 0);
        foo.w = rdmsr(HWP_REQUEST_MSR);
        mvwprintw(win, line++, 1, "MSR_HWP_REQUEST: %lx", foo.w);
        mvwprintw(win, line++, 1, "min_perf: %x", foo.s.min_perf);
        mvwprintw(win, line++, 1, "max_perf: %x", foo.s.max_perf);
        mvwprintw(win, line++, 1, "des_perf: %x", foo.s.des_perf);
        mvwprintw(win, line++, 1, "nrg_pref: %x", foo.s.nrg_pref);
        mvwprintw(win, line++, 1, "act_win: %x", foo.s.act_win);
        mvwprintw(win, line++, 1, "pkg_ctl: %x", foo.s.pkg_ctl);
        mvwprintw(win, line++, 1, "res1: %x", foo.s.res1);
        mvwprintw(win, line++, 1, "act_win_valid: %x", foo.s.act_win_valid);
        mvwprintw(win, line++, 1, "epp_valid: %x", foo.s.epp_valid);
        mvwprintw(win, line++, 1, "des_valid: %x", foo.s.des_valid);
        mvwprintw(win, line++, 1, "max_valid: %x", foo.s.max_valid);
        mvwprintw(win, line++, 1, "min_valid: %x", foo.s.min_valid);
        wnoutrefresh(win);
}

int main()
{
        WINDOW *wwin;
        WINDOW *fwin;
        WINDOW *cpuwin;
        WINDOW *memwin;
        WINDOW *fgwin;
        WINDOW *msrwin;
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

        cpuwin = newwin(15, 20, 0, 0);
        memwin = newwin(9, 20, 15, 0);
        wwin = newwin(12, 50, 0, 20);
        fwin = newwin(12, 50, 12, 20);
        fgwin = newwin(24,70,0,0);
        msrwin = newwin(24,70,0,0);

        while (1) {
                if(getch() == 'f')
                        mode = !mode;
                flushinp();

                if (mode == STATS_MODE) {
                        draw_cpu(cpuwin);
                        draw_mem(memwin);
                        draw_power(wwin, &wlist);
                        draw_freq(fwin, &flist);
                        doupdate();
                } else if (mode == GRID_MODE) {
                        draw_grid(fgwin);
                        //draw_HWP(msrwin);
                        doupdate();
                }

                usleep(DUR);
        }

        endwin();

        return 0;
}

