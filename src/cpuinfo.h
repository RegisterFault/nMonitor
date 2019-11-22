#ifndef CPUINFO_H
#define CPUINFO_H

struct meminfo {
        unsigned long total;
        unsigned long avail;
        unsigned long free;
        unsigned long cache;
};

long int get_sysfs_int(char*);
char * get_sysfs_string(const char *);
void get_cpuname(char **, int );
long int get_wattage(void);
long int get_freq(void);
void get_mem(struct meminfo *);
int get_bat_pct(void);
int get_turbo(void);
int get_cores(void);
int get_threads(void);
int get_temp(void);
char get_throttle_char(void);
int get_pl1(void);
int get_pl2(void);
double get_pkg_joules(void);
double get_pp0_joules(void);
double get_volt( unsigned int);
#endif
