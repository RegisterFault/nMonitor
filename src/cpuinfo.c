#define _GNU_SOURCE
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <glob.h>
#include "cpuinfo.h"
#include "msr.h"

/* arbitrary max frequency bounds to use when max freq of processor unfetchable */
#define MAX_FREQ_ARB 5000

char * BatteryPath = NULL; //populated by init_batinfo

int is_root(void)
{
        return geteuid() == 0;
}

//locate the primary BAT*
void init_batinfo(void)
{
        glob_t globbuf;
        char *battery_glob = "/sys/class/power_supply/BAT*";
        size_t len;
        
        glob(battery_glob, 0, NULL, &globbuf);
        
        if(globbuf.gl_pathc == 0){
                BatteryPath = NULL;
        } else {
                len = strlen(globbuf.gl_pathv[0]) + 1;
                /* BatteryPath only gets allocated once, and should never be freed except for termination*/
                BatteryPath = calloc(len, 1);
                /* glob automatically sorts results, so I should get the lowest-numbered BAT entry */
                strcpy(BatteryPath, globbuf.gl_pathv[0]); 
        }

        globfree(&globbuf);
        return;       
}

long int get_sysfs_int(char *path)
{
        FILE *f = fopen(path,"r");
        long int out;
        if (!f)
                return 0;
        if (fscanf(f, "%ld", &out) != 1)
                out = 0;
        fclose(f);
        return out;
}

/* always make sure to free the results of this function */
char *get_sysfs_string(const char *path)
{
        FILE *f = fopen(path, "r");
        char *out;
        if (!f)
                return NULL;;
        /* reminder that %m means scanf will malloc */
        if (fscanf(f, "%ms", &out) != 1)
                out = NULL;
        fclose(f);
        return out;
}

int is_amd()
{
        char *path = "/proc/cpuinfo";
        FILE *f = fopen(path, "r");
        size_t size = 1024;
        char *lbuf = malloc(size);
        char *name = NULL;
        int is_amd = 0;
        
        if (!f)
                return 0;

        while (getline(&lbuf, &size, f) != -1)
                if (strstr(lbuf, "vendor_id"))
                        break;
        sscanf(lbuf, "vendor_id\t: %ms", &name);

        fclose(f);
        free(lbuf);
        
        if (name == NULL)
                return is_amd;
        if (strcmp(name, "AuthenticAMD") == 0)
                is_amd = 1;
        free(name);
        
        return is_amd;
}

int get_cpu_family()
{
        char *path = "/proc/cpuinfo";
        FILE *f = fopen(path, "r");
        size_t size = 1024;
        char *lbuf = malloc(size);
        int out = 0;

        if(!f)
                return 0;

        while (getline(&lbuf, &size, f) != -1)
                if (strstr(lbuf, "cpu family"))
                        break;
        sscanf(lbuf, "cpu family\t: %d", &out);
        
        fclose(f);
        free(lbuf);
        return out;
}

/* 
 * I really, really shouldn't have to string-scan human-readable 
 * output in order to get this information....
 */
void get_amd_cpuname(char **str)
{
        char *path = "/proc/cpuinfo";
        FILE *f = fopen(path, "r");
        char *a, *b; //placeholder strings for scanf parsing
        size_t size = 1024;
        char *lbuf;
        char *scanf_out;
        int fam;

        if (!f)
                return;

        lbuf = malloc(size);
        
        fam = get_cpu_family();

        while (getline(&lbuf, &size, f) != -1)
                if (strstr(lbuf, "model name"))
                        break;

        if(fam == 21) {
                sscanf(lbuf, "model name\t: %*s %ms %ms", &a, &b);
                if (strcmp(a, "PRO") == 0) {
                        free(a);
                        scanf_out = b;
                } else {
                        free(b);
                        scanf_out = a;
                }
        } else if (fam == 23 ) {
                sscanf(lbuf, "model name\t: %*s %*s %*s %ms %ms", &a, &b);
                if (strcmp(a, "PRO") == 0){
                        free(a);
                        scanf_out = b;
                } else {
                        free(b);
                        scanf_out = a;
                }
        } else {
                sscanf(lbuf, "model name\t: %*s %*s %*s %ms", &a);
                        scanf_out = a;
        }
        
        if(scanf_out != NULL)
                *str = scanf_out;
        else
                free(scanf_out);

        fclose(f);
        free(lbuf);
        return;
}

/* mallocs output string, free after use */
void get_intel_cpuname(char **str)
{
        char *path = "/proc/cpuinfo";
        FILE *f = fopen(path, "r");
        size_t size = 1024;
        char *lbuf;
        char *scanf_pattern = "model name\t: %*s %*s %ms";
        char *scanf_buf;
        
        if (!f)
                return;

        lbuf = malloc(size);

        while (getline(&lbuf, &size, f) != -1)
                if (strstr(lbuf, "model name"))
                        break;
        sscanf(lbuf, scanf_pattern, &scanf_buf);

        if(scanf_buf != NULL)
               *str = scanf_buf; 
        free(lbuf);
        fclose(f);
}

char *get_cpuname()
{
        char *str = "";
        if (is_amd())
                get_amd_cpuname(&str);
        else 
                get_intel_cpuname(&str);
        return str;
}

int has_battery()
{
        return (BatteryPath != NULL) ? 1 : 0;
}

/* some systems report current charge, some report wattage */
int is_current()
{
        char *path = NULL;
        int ret;
      
        asprintf(&path,"%s/charge_full", BatteryPath);
        ret = (access(path, F_OK) == 0) ? 1 : 0;
        free(path);
        
        return ret;
}

int get_charge_pct()
{
        char *full = NULL;
        char *now = NULL;
        asprintf(&full, "%s/charge_full", BatteryPath); 
        asprintf(&now, "%s/charge_now", BatteryPath);
        
        float c_full = (float) get_sysfs_int(full);
        float c_now = (float) get_sysfs_int(now);

        free(full);
        free(now);

        if (c_full > 0.0 && c_now > 0.0)
                return (int) ((c_now / c_full) * 100);

        return 0;
}

/* this is in amp-hours, not watts */
float get_charge_full()
{
        char *path = NULL;
        asprintf(&path, "%s/charge_full", BatteryPath);
        
        float ret = ((float) get_sysfs_int(path)) / 1000000.0;
        
        free(path);
        return ret;
}

float get_charge_full_design()
{
        char *path = NULL;
        asprintf(&path, "%s/charge_full_design", BatteryPath);
        
        float ret = ((float) get_sysfs_int(path)) / 1000000.0;

        free(path);
        return ret;
}

/* we return milliwatts */
int get_charge_wattage()
{
        char *cur = NULL;
        char *volt = NULL;
        asprintf(&cur, "%s/current_now", BatteryPath);
        asprintf(&volt, "%s/voltage_now", BatteryPath);
        float c_now = (float) get_sysfs_int(cur);
        float v_now = (float) get_sysfs_int(volt);
        
        free(cur);
        free(volt);

        float wattage = (c_now / 1000000.0) * (v_now / 1000000.0);

        return (int) (wattage * 1000);
}

long int get_wattage(void)
{
        char *path = NULL;
        long int ret;
        asprintf(&path, "%s/power_now", BatteryPath);
        
        ret = get_sysfs_int(path) / 1000;

        free(path);
        return ret;
}

long int get_freq()
{
        char * path = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";
        return get_sysfs_int(path) / 1000;
}

/* need to free the result of this function */
char *get_governor()
{
        char *path = "/sys/devices/system/cpu/cpufreq/policy0/scaling_governor";
        return get_sysfs_string(path);
}

int get_bat_pct()
{
        char *energy_full = NULL;
        char *energy_now = NULL; 
        asprintf(&energy_full, "%s/energy_full", BatteryPath);
        asprintf(&energy_now, "%s/energy_now", BatteryPath);
        
        float e_full = (float) get_sysfs_int(energy_full);
        float e_now = (float) get_sysfs_int(energy_now);

        free(energy_full);
        free(energy_now);
        
        if (e_now > 0 && e_full > 0)
                return (int) ((e_now / e_full) * 100);
                
        return 0;
}

int get_bat_full()
{
        char *path = NULL;
        int ret;
        asprintf(&path, "%s/energy_full", BatteryPath);
        
        ret = get_sysfs_int(path) / 1000000;

        free(path);
        return ret;
}

int get_bat_design()
{
        char *path = NULL;
        int ret;
        asprintf(&path, "%s/energy_full_design", BatteryPath);
        ret = get_sysfs_int(path) / 1000000;

        free(path);
        return ret;
}

void get_mem(struct meminfo *in)
{
        char *path = "/proc/meminfo";
        FILE *f = fopen(path,"r");
        if (!f)
                return;
        int size = 1024;
        char *lbuf = malloc(size);
       
        /* we use all members for parsing condition, set to zero */
        memset(in, 0, sizeof(struct meminfo)); 

        while (getline(&lbuf,(size_t *)&size, f) != -1){
                if (strstr(lbuf, "MemTotal:"))
                        sscanf(lbuf, "MemTotal:\t%lu", &(in->total));
                if (strstr(lbuf,"MemFree:"))
                        sscanf(lbuf, "MemFree:\t%lu", &(in->free));
                if (strstr(lbuf,"MemAvailable:")) 
                        sscanf(lbuf, "MemAvailable:\t%lu", &(in->avail));
                if (strstr(lbuf,"Cached:")) 
                        sscanf(lbuf, "Cached:\t%lu", &(in->cache));
                if (in->free && in->total && in->avail && in->cache)
                        break; /*stop parsing file, we have what we need */
        }

        free(lbuf);
        fclose(f);
}

int get_turbo()
{       
        return is_amd() ? get_amd_boost() : get_intel_boost();
}

int get_amd_boost()
{
        char *path = "/sys/devices/system/cpu/cpufreq/boost";
        return get_sysfs_int(path);        
}

int get_intel_boost()
{
        /* no_turbo returns 1 if turbo is off */
        char *path = "/sys/devices/system/cpu/intel_pstate/no_turbo";
        return get_sysfs_int(path) ? 0 : 1; 
}

int get_boost_freq()
{
        if(is_root() && !is_amd())
                return get_intel_boost_freq();
        else
                return MAX_FREQ_ARB;
}

int have_cpuid()
{
        return (access("/dev/cpu/0/cpuid", F_OK) != -1) ? 1 : 0;
}

/* requires root */
int get_intel_freq(int freq)
{
       char *path = "/dev/cpu/0/cpuid";
       int fd = open(path, O_RDONLY);
       struct cpuid_result r;
       union cpuid_access a;

       a.eax = CPUID_FREQ_OFFSET;
       a.ecx = 0;

       if (fd < 0)
               return 0;

       lseek(fd, a.w, SEEK_SET);

       if (read(fd, &r, 16) != 16) {
               close(fd);
               return 0;
       }

       close(fd);

       switch(freq){
       case INTEL_BASE:
                return r.a;
                break;
       case INTEL_BOOST:
                return r.b;
                break;
       default:
                return 0;
                break;
       }
}

int get_intel_boost_freq()
{
        return get_intel_freq(INTEL_BOOST);
}

int get_base_freq()
{
        return get_intel_base_freq();
}

int get_intel_base_freq()
{
        return get_intel_freq(INTEL_BASE);
}

int get_cores()
{
        char *path = "/proc/cpuinfo";
        FILE *f = fopen(path,"r");
        size_t size = 1024;
        char *lbuf = malloc(size);
        int cores = 0;
        if (!f)
                return 0;

        while (getline(&lbuf, &size, f) != -1){
                if(strstr(lbuf, "cpu cores")){
                        if(sscanf(lbuf, "cpu cores\t: %d", &cores) != 1)
                                cores = 0;
                        break;
                }
        }

        fclose(f);
        free(lbuf);
        return cores;
}

int get_threads()
{
        char *path = "/proc/cpuinfo";
        FILE *f = fopen(path,"r");
        size_t size = 1024;
        char *lbuf = malloc(size);
        int threads = 0;
        if (!f)
                return 0;
        while (getline(&lbuf, &size, f) != -1){
                if (strstr(lbuf, "siblings")){
                        if(sscanf(lbuf, "siblings\t: %d", &threads) != 1)
                                threads = 0;
                        break;
                }
        }

        fclose(f);
        free(lbuf);
        return threads;
}

int get_temp()
{
        return is_amd() ? get_amd_temp() : get_intel_temp();
}

int get_amd_temp()
{
        char *therm_pattern = "/sys/class/hwmon/hwmon*/name";
        glob_t therm_glob;
        char *name_path;
        char *temp_path = malloc(100);
        int temp = 0;
        int i;

        if (glob(therm_pattern, 0, NULL, &therm_glob) != 0)
                goto cleanup;

        for (i = 0; i < therm_glob.gl_pathc; i++){
                name_path = get_sysfs_string(therm_glob.gl_pathv[i]);
                if (strcmp(name_path, "k10temp") == 0){
                        free(name_path);
                        break; /* found hwmon entry */
                }
                free(name_path);
        }

        snprintf(temp_path, 100, "/sys/class/hwmon/hwmon%d/temp1_input", i);
        temp = get_sysfs_int(temp_path)/1000;


cleanup:
        globfree(&therm_glob);
        free(temp_path);
        return temp;
}

int get_intel_temp()
{
        /* find x86_pkg_temp */
        char *therm_pattern = "/sys/class/thermal/thermal_zone*/type";
        glob_t therm_glob;
        char *type_path;
        char *temp_path = malloc(100);
        int temp = 0;
        int i;
        
        bzero(temp_path, 100);
        
        if (glob(therm_pattern, 0, NULL, &therm_glob) != 0)
                goto cleanup;

        for (i = 0; i < therm_glob.gl_pathc; i++){
                type_path = get_sysfs_string(therm_glob.gl_pathv[i]);
                if (strcmp(type_path, "x86_pkg_temp") == 0){
                        free(type_path);
                        break; /* found thermal zone for proc */
                }
                free(type_path);
        }

        /* i is the correct thermal zone number */
        snprintf(temp_path, 100, "/sys/class/thermal/thermal_zone%d/temp", i);
        temp = get_sysfs_int(temp_path) / 1000;

cleanup:
        globfree(&therm_glob);
        free(temp_path);
        return temp;
}

/* requires root */
char get_throttle_char()
{
        LIMITS a;
        a.w = rdmsr(LIMITS_MSR);

        if (a.s.thermal || a.s.ratl || a.s.vreg || a.s.prochot)
                return 'T';
        if (a.s.cur)
                return 'C';
        if (a.s.pl1 || a.s.pl2)
                return 'P';
        if (a.s.max_turbo || a.s.ttas)
                return 'B';

        return ' ';
}

/* requires root */
int get_pl1()
{
        PPLC a;
        a.w = rdmsr(PPLC_MSR);
        return (int) (a.s.pl1_value * 0.032);
}

/* requires root */
int get_pl2()
{
        PPLC a;
        a.w = rdmsr(PPLC_MSR);
        return (int) (a.s.pl2_value * 0.032);
}

/* requires root */
double get_pkg_joules()
{
        return is_amd() ? get_amd_pkg_joules() : get_intel_pkg_joules();
}

/* requires root */
double get_intel_pkg_joules()
{
        RAPLU u;
        NRGP e;

        u.w = rdmsr(RAPLU_MSR);
        e.w = rdmsr(NRGP_MSR);

        return (double) (e.s.energy * (pow(0.5, (double)u.s.es_units)));
}

/* requires root */
double get_amd_pkg_joules()
{
        AMD_RAPLU u;
        AMD_NRGP e;

        u.w = rdmsr(AMD_RAPLU_MSR);
        e.w = rdmsr(AMD_NRGP_MSR);

        return (double) (e.s.energy * (pow(0.5, (double) u.s.energy_units)));
}

/* requires root */
double get_pp0_joules()
{
        RAPLU u;
        NRG0 e;

        u.w = rdmsr(RAPLU_MSR);
        e.w = rdmsr(NRG0_MSR);

        return (double) (e.s.energy * (pow(0.5, (double) u.s.es_units)));
}

/* requires root */
double get_volt( unsigned int plane)
{
        VOLT v;
        if (plane > 4)
                return 0.0;

        /* have to write which plane to read */
        v.w = VOLT_READ;
        v.s.plane = plane;
        wrmsr(VOLT_MSR, v.w);

        v.w = rdmsr(VOLT_MSR);
        return v.s.volt / 1.024;
}       
