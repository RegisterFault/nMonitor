#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <glob.h>
#include "cpuinfo.h"
#include "msr.h"

long int get_sysfs_int(char *path)
{
        FILE *f = fopen(path,"r");
        long int out;
        if (!f)
                return 0;
        if (fscanf(f, "%lld", &out) != 1)
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
        if (!f)
                return 0;
        size_t size = 1024;
        char *lbuf = malloc(size);
        char *name = NULL;
        int is_amd = 0;
        
        while (getline(&lbuf, &size, f)){
                if (strstr(lbuf, "vendor_id")){
                        sscanf(lbuf, "vendor_id\t: %ms", &name);
                        break;
                }
        }
                                
        if (!name)
                goto exit;
        if (strcmp(name, "AuthenticAMD") == 0)
                is_amd = 1;
        
        free(name);

exit:
        fclose(f);
        free(lbuf);
        return is_amd;
}

void get_cpuname(char **str, int in_size)
{
        char *path = "/proc/cpuinfo";
        FILE *f = fopen(path, "r");
        if (!f)
                return;
        int size = 1024;
        char *lbuf = malloc(size);
        char *name;
        char *scanf_pattern;

        if (is_amd())
                scanf_pattern = "model name\t: %*s %*s %*s %ms";
        else
                scanf_pattern = "model name\t: %*s %*s %ms";

        while (getline(&lbuf, (size_t *)&size, f)){
                if (strstr(lbuf, "model name")){
                        sscanf(lbuf, scanf_pattern,&name);
                        break;
                }
        }

        strncpy(*str, name, in_size);
        free(lbuf);
        free(name);
        fclose(f);
}

long int get_wattage(void)
{
        char *path = "/sys/class/power_supply/BAT0/power_now";
        return get_sysfs_int(path) / 1000;
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
        char *energy_full = "/sys/class/power_supply/BAT0/energy_full";
        char *energy_now = "/sys/class/power_supply/BAT0/energy_now";
        float e_full = (float) get_sysfs_int(energy_full);
        float e_now = (float) get_sysfs_int(energy_now);
        
        if (e_now > 0 && e_full > 0)
                return (int) ((e_now / e_full) * 100);
                
        return 0;
}

int get_bat_full()
{
        char *path = "/sys/class/power_supply/BAT0/energy_full";
        return get_sysfs_int(path) / 1000000;
}

int get_bat_design()
{
        char *path = "/sys/class/power_supply/BAT0/energy_full_design";
        return get_sysfs_int(path) / 1000000;
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

        while (getline(&lbuf,(size_t *)&size, f)){
                if (strstr(lbuf, "MemTotal:"))
                        sscanf(lbuf, "MemTotal:\t%lld", &(in->total));
                if (strstr(lbuf,"MemFree:"))
                        sscanf(lbuf, "MemFree:\t%lld", &(in->free));
                if (strstr(lbuf,"MemAvailable:")) 
                        sscanf(lbuf, "MemAvailable:\t%lld", &(in->avail));
                if (strstr(lbuf,"Cached:")) 
                        sscanf(lbuf, "Cached:\t%lld", &(in->cache));
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

int get_cores()
{
        char *path = "/proc/cpuinfo";
        FILE *f = fopen(path,"r");
        size_t size = 1024;
        char *lbuf = malloc(size);
        int cores = 0;
        if (!f)
                return 0;

        while (getline(&lbuf, &size, f)){
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
        while (getline(&lbuf, &size, f)){
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
