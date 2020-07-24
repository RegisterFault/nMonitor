#ifndef CPUINFO_H
#define CPUINFO_H
#include <stdint.h>


struct meminfo {
        uint64_t total;
        uint64_t avail;
        uint64_t free;
        uint64_t cache;
};

#define CPUID_FREQ_OFFSET (0x16)
union cpuid_access {
        struct {
                uint32_t eax;
                uint32_t ecx;
        } __attribute__((packed));
        uint64_t w;
};

struct cpuid_result {
        uint32_t a;
        uint32_t b;
        uint32_t c;
        uint32_t d;
} __attribute__((packed));

int is_root(void);
void init_batinfo(void);
long int get_sysfs_int(char*);
char *get_sysfs_string(const char *);
int is_amd(void);
int get_cpu_family(void);
void get_amd_cpuname(char **);
void get_intel_cpuname(char **);
void get_cpuname(char **);
long int get_wattage(void);
char *get_governor(void);
long int get_freq(void);
void get_mem(struct meminfo *);
int has_battery(void);
int is_current(void);
int get_charge_pct(void);
float get_charge_full(void);
float get_charge_full_design(void);
int get_charge_wattage(void);
int get_bat_pct(void);
int get_bat_full(void);
int get_bat_design(void);
int get_turbo(void);
int get_amd_boost(void);
int get_intel_boost(void);
int get_boost_freq(void);
#define INTEL_BASE 0
#define INTEL_BOOST 1
int have_cpuid(void);
int get_intel_freq(int);
int get_base_freq(void);
int get_intel_base_freq(void);
int get_intel_boost_freq(void);
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
