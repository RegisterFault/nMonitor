#define _GNU_SOURCE
#include <ncurses.h>
#include <unistd.h>
#include <fcntl.h>
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
                        draw_power(app.bat0_win, &app.bat0_list, 0);
                        draw_power(app.bat1_win, &app.bat1_list, 1);
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
