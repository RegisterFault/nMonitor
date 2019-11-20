#ifndef CPUINFO_H
#define CPUINFO_H

struct meminfo {
        unsigned long total;
        unsigned long avail;
        unsigned long free;
        unsigned long cache;
};

void get_cpuname(char **, int );
long int get_wattage(void);
long int get_freq(void);
void get_mem(struct meminfo *);
int get_bat_pct(void);
#endif
