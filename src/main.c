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

struct application app;

int main()
{
        init_app();
        signal(SIGWINCH, handle_resize);

        while (1) {
                if (getch() == 'f')
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
