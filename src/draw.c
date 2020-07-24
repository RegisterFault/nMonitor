#include "draw.h"

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
        
        trunc_list(list, cols - 2);
        draw_points(win, list, max, '*'); 
}

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
        mvwprintw(win, 0, 0, "%ld mW -- Battery: %d%% -- Capacity: %d/%d Wh",
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
        mvwprintw(win, 0, 0, "%ld MHz", last_elem(*list)->data);

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
        box(win, 0, 0);
        
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