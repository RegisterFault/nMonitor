#include "draw.h"

void draw_app()
{
        extern struct application app;
        /* ncurses setup */
        initscr();
        curs_set(0);
        noecho();
        cbreak();
        nodelay(stdscr, 1);

        app.cpuwin = newwin(15, 25, 0, 0);
        app.memwin = newwin(9, 25, 15, 0);
        app.wwin   = newwin(12, 45, 0, 25);
        app.fwin   = newwin(12, 45, 12, 25);
        app.fgwin  = newwin(24, 70, 0, 0);
}

void init_app()
{
        extern struct application app;
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

void usleep_nointr(unsigned long micro)
{
        struct timespec dur = {.tv_sec = 0, .tv_nsec = micro * 1000 };
        struct timespec rem = {.tv_sec = 0, .tv_nsec = 0 };
        while (nanosleep(&dur, &rem) == -1){
                if (errno == EINTR)
                        dur.tv_nsec = rem.tv_nsec;
                else
                        return;
        }
}

void debug_print(const char *fmt, ...)
{
        va_list dbg_list;
        va_start(dbg_list, fmt);
        move(24,0);
        vw_printw(stdscr, fmt, dbg_list);
        va_end(dbg_list);
        refresh();
}

int calc_y(long int max, long int lines, long int in)
{
        float a = in;
        float b = max;
        float c = lines;
        return (int) (c - ((a / b) * c)) -1;
}

void draw_points(WINDOW *win, struct node **list, int max, char point)
{
        int lines = 0;
        int cols = 0;
        int x,y;
        struct node *cur;

        getmaxyx(win, lines, cols);
        lines -= 2; //account for box
        
        for(x = 0, cur = *list; cur->next != NULL; cur = cur->next){
                for(y = calc_y(max, lines, cur->data); y < lines; y++)
                        mvwprintw(win, y + 1, x + 1, "%c", point);
                x++;
        }
}

void draw_graph(WINDOW *win, struct node **list, long int max)
{
        int lines = 0;
        int cols = 0;
        
        getmaxyx(win, lines, cols);
        wclear(win);
        box(win, 0, 0);
       
        if(*list == NULL)
               return; 
        trunc_list(list, cols - 1);
        draw_points(win, list, max, '*'); 
}

void test_grid(WINDOW *win)
{
        int row = 1;
        int col = 1;
        int total = 256;
        int iterations = 0;
        int msgsz = 10;
        int x,y;

        getmaxyx(win, y, x);

        box(win, 0, 0);
        mvwprintw(win, 0, 0, "CPU FREQUENCY LISTING");
        
        while(iterations < total){
                mvwprintw(win, row, col, "%3d: %4d", iterations, 5000);
                iterations++;
                row++;
                if(row > (y-2)){
                        row = 1;
                        col += msgsz;
                        if ((col + msgsz) > (x - 2))
                                break;
                }
        }
        wnoutrefresh(win);
}

void draw_grid(WINDOW *win)
{
        int threads = get_threads(); 
        int freq;
        int i = 0;
        int row = 1; //row, col set to 1 to index loop inside window box
        int col = 1;
        int strsz = 10; //current size of cpu display string
        int y,x; // using these to fetch window size.

        getmaxyx(win, y, x);

        box(win, 0, 0);
        mvwprintw(win, 0, 0, "CPU FREQUENCY LISTING");

        while (i < threads) {
                freq = get_cur_freq(i);
                mvwprintw(win, row, col, "%3d: %4d", i, freq);
                i++;
                row++;
                if (row > (y - 2)){
                        row = 1;
                        col += strsz;
                        if ((col + strsz) > (x - 2)) //if the next column goes off the bounds
                                break;
                }
        }

        wnoutrefresh(win);
}
void draw_amperage(WINDOW *win, struct node **list)
{
        mvwprintw(win, 0, 0, "%ld mW -- [%c%c] -- Bat %d%% -- Cap %2.1f/%2.1f Ah",
                  last_elem(*list)->data,
                  ac_attached() ? 'A' : ' ',
                  is_charging() ? 'C' : ' ',
                  get_charge_pct(),
                  get_charge_full(),
                  get_charge_full_design());
}

void draw_wattage(WINDOW *win, struct node **list)
{
        mvwprintw(win, 0, 0, "%ld mW -- [%c%c] -- Bat %d%% -- Cap %d/%d Wh",
                  last_elem(*list)->data,
                  ac_attached() ? 'A' : ' ',
                  is_charging() ? 'C' : ' ',
                  get_bat_pct(),
                  get_bat_full(),
                  get_bat_design());
}

void draw_power(WINDOW *win, struct node **list)
{       
        /* some systems report Amp-Hours, some report Watt-Hours */
        if (is_current()){
                add_node(list, get_charge_wattage());
                draw_graph(win, list, 35000);
                draw_amperage(win, list);
        } else {
                add_node(list, get_wattage());
                draw_graph(win, list, 35000);
                draw_wattage(win, list);
        }
        wnoutrefresh(win);
}

void draw_freq(WINDOW *win, struct node **list)
{
        int max_boost = get_boost_freq();
        if(max_boost < 100) /*arbitrarily low and invalid number*/
                max_boost = 5000;

        add_node(list, get_freq());
        draw_graph(win, list, max_boost);
        mvwprintw(win, 0, 0, "%ld MHz", last_elem(*list)->data);

        wnoutrefresh(win);
}

void draw_cpu(WINDOW *win)
{
        int line = 0;
        static double last_pkg_nrg = 0.0;
        static double last_pp0_nrg = 0.0;
        static double last_pp1_nrg = 0.0;
        static double last_dram_nrg = 0.0;
        double cur_pkg_nrg = 0.0;
        double cur_pp0_nrg = 0.0;
        double cur_pp1_nrg = 0.0;
        double cur_dram_nrg = 0.0;
        char *governor = get_governor();
        char *cpu_name = get_cpuname();
        char *cpu_brand = CPU_BRAND_STR();

        wclear(win);
        box(win, 0, 0);

        mvwprintw(win, line++, 0, "CPU INFO: %s", cpu_brand);
        mvwprintw(win, line++, 1, "Model: %s ", cpu_name);
        mvwprintw(win, line++, 1, "[ %dS %4dC %4dT ]",
                  get_sockets(),
                  get_cores(),
                  get_threads());
        mvwprintw(win, line++, 1, "Temp: %dC", get_temp());
        mvwprintw(win, line++, 1, "Boost: %s ", get_turbo() ? "on" : "off" );
        mvwprintw(win, line++, 1, "Gov: %s", governor);

        if (is_root()){
                if(!have_msr()){
                        mvwprintw(win, line++, 1, "(msr not detected)");
                        goto cleanup;
                }

                cur_pkg_nrg = get_pkg_joules();
                if(!is_amd()){
                        cur_pp0_nrg = get_pp0_joules();
                        cur_pp1_nrg = get_pp1_joules();
                        cur_dram_nrg = get_dram_joules();
                }

                double pkg_watts = (last_pkg_nrg == 0.0) ? 0.0 :
                                   (cur_pkg_nrg - last_pkg_nrg) / DUR_SEC;

                if (!is_amd())
                        mvwprintw(win, line++, 1, "Throttle: %c", get_throttle_char());

                mvwprintw(win, line++, 1, "PKG:    %6.2f W", pkg_watts);

                if(!is_amd()){
                        double cpu_watts = (last_pp0_nrg == 0.0) ? 0.0 : 
                                           (cur_pp0_nrg - last_pp0_nrg) / DUR_SEC;
                        double uncore_watts = (last_pp1_nrg == 0.0) ? 0.0 :
                                              (cur_pp1_nrg - last_pp1_nrg) / DUR_SEC;
                        double dram_watts = (last_dram_nrg == 0.0)? 0.0 :
                                            (cur_dram_nrg - last_dram_nrg) / DUR_SEC;
                        mvwprintw(win, line++, 1, "CPU:    %6.2f W", cpu_watts);
                        mvwprintw(win, line++, 1, "UNCORE: %6.2f W", uncore_watts);
                        mvwprintw(win, line++, 1, "DRAM:   %6.2f W", dram_watts);
                }


                if (!is_amd() && hwp_enabled())
                        mvwprintw(win, line++, 1, "HWP Pref:   0x%02x", get_hwp_pref());

                /*if (!is_amd()) {
                        mvwprintw(win, line++, 1, "CPU:   %+2.2f mV", get_volt(CPU_PLANE));
                        mvwprintw(win, line++, 1, "Cache: %+2.2f mV", get_volt(CACHE_PLANE));
                }*/ //This generates too many errors in syslog

                last_pkg_nrg = cur_pkg_nrg;
                last_pp0_nrg = cur_pp0_nrg;
                last_pp1_nrg = cur_pp1_nrg;
                last_dram_nrg = cur_dram_nrg;

        }
cleanup:
        free(cpu_name);
        free(governor);
        wnoutrefresh(win);
}

void draw_mem(WINDOW *win)
{
        struct meminfo mem;
        int line = 0;
        
        get_mem(&mem);
        
        wclear(win);
        box(win, 0, 0);
        
        // /proc/cpuinfo reports in KiB, despite actually showing kB 
        mvwprintw(win, line++, 0, "MEM INFO");
        mvwprintw(win, line++, 1, "Total: %5ld MiB", mem.total / 1024);
        mvwprintw(win, line++, 1, "Avail: %5ld MiB", mem.avail / 1024);
        mvwprintw(win, line++, 1, "Free:  %5ld MiB", mem.free / 1024);
        mvwprintw(win, line++, 1, "Cache: %5ld MiB", mem.cache / 1024);
        /* this appears to be inaccurate */
        mvwprintw(win, line++, 1, "Used:  %5ld MiB", (mem.total - mem.avail) / 1024);
        wnoutrefresh(win);
}
