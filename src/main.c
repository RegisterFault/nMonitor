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
                if (getch() == 'f') {
                        app.mode = !app.mode;
				}
				else if (getch() == 'q'){
						endwin();
						exit(1);
				}
                flushinp();

                if (app.mode == STATS_MODE) {
						refresh_batinfo();
                        draw_cpu(app.cpuwin);
                        draw_mem(app.memwin);
						if(has_battery(0))
							draw_power(app.bat0_win, &app.bat0_list, 0);
						else {
							werase(app.bat0_win);
							wrefresh(app.bat0_win);
						}
						if(has_battery(1))
							draw_power(app.bat1_win, &app.bat1_list, 1);
						else {
							werase(app.bat1_win);
							wrefresh(app.bat1_win);
						}
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
