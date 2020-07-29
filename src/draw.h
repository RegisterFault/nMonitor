#ifndef DRAW_H
#define DRAW_H
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

void debug_print(const char *, ...);
int calc_y(long int, long int, long int);
void draw_points(WINDOW *, struct node **, int, char);
void draw_graph(WINDOW *, struct node **, long int);
void test_grid(WINDOW *);
void draw_grid(WINDOW *);
void draw_amperage(WINDOW *, struct node **);
void draw_wattage(WINDOW *, struct node **);
void draw_power(WINDOW *, struct node **);
void draw_freq(WINDOW *, struct node **);
void draw_cpu(WINDOW *);
void draw_mem(WINDOW *);

#endif
