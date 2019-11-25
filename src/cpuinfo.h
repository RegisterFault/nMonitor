#ifndef CPUINFO_H
#define CPUINFO_H

struct meminfo {
        unsigned long total;
        unsigned long avail;
        unsigned long free;
        unsigned long cache;
};

long int get_sysfs_int(char*);
char *get_sysfs_string(const char *);
int is_amd(void);
void get_cpuname(char **, int );
long int get_wattage(void);
char *get_governor(void);
long int get_freq(void);
void get_mem(struct meminfo *);
int get_bat_pct(void);
int get_bat_full(void);
int get_bat_design(void);
int get_turbo(void);
int get_amd_boost(void);
int get_intel_boost(void);
int get_cores(void);
int get_threads(void);
int get_temp(void);
int get_amd_temp(void);
int get_intel_temp(void);
char get_throttle_char(void);
int get_pl1(void);
int get_pl2(void);
double get_pkg_joules(void);
double get_intel_pkg_joules(void);
double get_amd_pkg_joules();
double get_pp0_joules(void);
double get_volt( unsigned int);
#endif
