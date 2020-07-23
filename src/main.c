#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "cpuinfo.h"
#include "list.h"
#include "msr.h"

/* hard-coded update interval, in microseconds */
#define DUR (500000L)

/* used in logic of display routine */
#define DUR_SEC (DUR*0.000001)

void draw_amperage(WINDOW *win, struct node *list)
{
        mvwprintw(win, 0, 0, "%ld mW -- Battery: %d%% -- Capacity: %2.1f/%2.1f Ah",
                        last_elem(list)->data,
                        get_charge_pct(),
                        get_charge_full(),
                        get_charge_full_design());
        wrefresh(win);
        add_node(list, get_charge_wattage());
}

void draw_wattage(WINDOW *win, struct node *list)
{
        mvwprintw(win,0,0,"%ld mW -- Battery: %d%% -- Capacity: %d/%d Wh",
                last_elem(list)->data,
                get_bat_pct(),
                get_bat_full(),
                get_bat_design());
        wrefresh(win);
        add_node(list, get_wattage());
}

void draw_power(WINDOW *win, struct node **list)
{       
        draw_graph(win, list, 35000);
        /* some systems report Amp-Hours, some report Watt-Hours */
        if (is_current())
                draw_amperage(win, *list);
        else
                draw_wattage(win, *list);
}

void draw_freq(WINDOW *win, struct node **list)
{
        int max_boost = get_boost_freq();

        draw_graph(win, list, max_boost);
        mvwprintw(win,0,0,"%ld MHz",last_elem(*list)->data);

        wrefresh(win);
        add_node(*list, get_freq());
}

void draw_cpu(WINDOW *win)
{
        char *cpu_name = malloc(20);
        int line = 0;
        static double last_pkg_nrg = 0;
        char *governor = get_governor();
        char *cpu_brand;
        
        bzero(cpu_name,20);
        get_cpuname(&cpu_name, 20);
        cpu_brand = is_amd() ? "AMD" : "Intel";

        wclear(win);
        box(win, 0, 0);

        mvwprintw(win, line++, 0, "CPU INFO: %s", cpu_brand);
        mvwprintw(win, line++, 1, "%s %dC/%dT",
                        cpu_name,
                        get_cores(),
                        get_threads());
        mvwprintw(win, line++, 1, "Temp: %dC", get_temp());
        mvwprintw(win, line++, 1, "Boost: %s ", get_turbo() ? "on" : "off" );
        mvwprintw(win, line++, 1, "Gov: %s", governor);
        if (is_root() && have_msr()){ 
                if (last_pkg_nrg != 0)
                        mvwprintw(win, line++, 1, "PKG:   %6.2f W",
                                        (get_pkg_joules() - last_pkg_nrg) / DUR_SEC);
                else
                        mvwprintw(win, line++, 1, "PKG:   %6.2f W", 0.0);
                last_pkg_nrg = get_pkg_joules();
                if (!is_amd()) {
                        mvwprintw(win, line++, 1, "Throttle: %c", get_throttle_char());
                        if (have_cpuid()) {
                                mvwprintw(win, line++, 1, "Base:  %ld MHz", get_base_freq());
                                mvwprintw(win, line++, 1, "Boost: %ld MHz", get_boost_freq());
                        }
                        mvwprintw(win, line++, 1, "CPU:   %+2.2f mV", get_volt(CPU_PLANE));
                        mvwprintw(win, line++, 1, "Cache: %+2.2f mV", get_volt(CACHE_PLANE));
                }

        }
        free(governor);
        wrefresh(win);
}

void draw_mem(WINDOW *win)
{
        struct meminfo mem;
        int line = 0;
        
        get_mem(&mem);
        wclear(win);
        box(win,0,0);
        
        // /proc/cpuinfo reports in KiB, despite actually showing kB 
        mvwprintw(win, line++, 0, "MEM INFO");
        mvwprintw(win, line++, 1, "Total: %5ld MiB", mem.total / 1024);
        mvwprintw(win, line++, 1, "Avail: %5ld MiB", mem.avail / 1024);
        mvwprintw(win, line++, 1, "Free:  %5ld MiB", mem.free / 1024);
        mvwprintw(win, line++, 1, "Cache: %5ld MiB", mem.cache / 1024);
        /* this appears to be inaccurate */
        mvwprintw(win, line++, 1, "Used:  %5ld MiB", (mem.total - mem.avail) / 1024);
        
        wrefresh(win);
}


int main()
{
        WINDOW *wwin;
        WINDOW *fwin;
        WINDOW *cpuwin;
        WINDOW *memwin;
        struct node *wlist = init_node();
        struct node *flist = init_node();

        init_batinfo();

        initscr();
        curs_set(0);

        cpuwin = newwin(14, 20, 0, 0);
        memwin = newwin(10, 20, 14, 0);
        
        wwin = newwin(12, 50, 0, 20);
        fwin = newwin(12, 50, 12, 20);

        /* this helps reduce glitchiness during initial loading */
        refresh();

        while (1) {
                draw_cpu(cpuwin);
                draw_mem(memwin);
                draw_power(wwin, &wlist);
                draw_freq(fwin, &flist);
                
                refresh();
                usleep(DUR);
        }

        endwin();

        return 0;
}

