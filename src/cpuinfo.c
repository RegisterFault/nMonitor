#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <glob.h>
#include "cpuinfo.h"

long int get_sysfs_int(char *path)
{
        FILE * f = fopen(path,"r");
        long int out;
        if(!f)
                return 0;
        if(fscanf(f, "%lld", &out) != 1)
                out = 0;
        fclose(f);
        return out;
}

/* always make sure to free the results of this function */
char * get_sysfs_string(const char * path)
{
        FILE * f = fopen(path, "r");
        char * out;
        if(!f)
                return NULL;;
        /* reminder that %m means scanf will malloc */
        if(fscanf(f,"%ms",&out) != 1)
                out = NULL;
        fclose(f);
        return out;
}

void get_cpuname(char **str, int in_size)
{
        char * path = "/proc/cpuinfo";
        FILE * f = fopen(path, "r");
        if(!f)
                return;
        int size = 1024;
        char *lbuf = malloc(size);
        char *name;

        while(getline(&lbuf, (size_t *)&size, f)){
                if(strstr(lbuf,"model name")){
                        sscanf(lbuf, "model name\t: %*s %*s %ms",&name);
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
        char * path = "/sys/class/power_supply/BAT0/power_now";
        return get_sysfs_int(path)/1000;
}

long int get_freq()
{
        char * path = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";
        
        return get_sysfs_int(path)/1000;
}

int get_bat_pct()
{
        char *energy_full = "/sys/class/power_supply/BAT0/energy_full";
        char *energy_now = "/sys/class/power_supply/BAT0/energy_now";
        float e_full = (float) get_sysfs_int(energy_full);
        float e_now = (float) get_sysfs_int(energy_now);
        
        if(e_now > 0 && e_full > 0)
                return (int) ((e_now/e_full)*100);
                
        return 0;
}

void get_mem(struct meminfo * in)
{
        char * path = "/proc/meminfo";
        FILE * f = fopen(path,"r");
        if(!f)
                return;
        int size = 1024;
        char * lbuf = malloc(size);
      
        memset(in,0,sizeof(struct meminfo)); /* we use all members for parsing condition, set to zero */

        while(getline(&lbuf,(size_t *)&size, f)){
                if(strstr(lbuf,"MemTotal:"))
                        sscanf(lbuf,"MemTotal:\t%lld",&(in->total));
                if(strstr(lbuf,"MemFree:"))
                        sscanf(lbuf, "MemFree:\t%lld", &(in->free));
                if(strstr(lbuf,"MemAvailable:")) 
                        sscanf(lbuf, "MemAvailable:\t%lld", &(in->avail));
                if(strstr(lbuf,"Cached:")) 
                        sscanf(lbuf, "Cached:\t%lld", &(in->cache));
                if(in->free && in->total && in->avail && in->cache)
                        break; /*stop parsing file, we have what we need */
        }

        free(lbuf);
        fclose(f);
}

int get_turbo()
{       
        /* no_turbo is 1 if turbo is off. returning 0 if off */
        char * path = "/sys/devices/system/cpu/intel_pstate/no_turbo";
        return get_sysfs_int(path) ? 0 : 1;
}

int get_cores()
{
        char * path = "/proc/cpuinfo";
        FILE * f = fopen(path,"r");
        size_t size = 1024;
        char * lbuf = malloc(size);
        int cores = 0;
        if(!f)
                return 0;

        while(getline(&lbuf, &size, f)){
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
        char * path = "/proc/cpuinfo";
        FILE * f = fopen(path,"r");
        size_t size = 1024;
        char * lbuf = malloc(size);
        int threads = 0;
        if(!f)
                return 0;
        while(getline(&lbuf, &size, f)){
                if(strstr(lbuf, "siblings")){
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
        /* find x86_pkg_temp */
        char * therm_pattern = "/sys/class/thermal/thermal_zone*/type";
        glob_t therm_glob;
        char * type_path;
        char * temp_path = malloc(100);
        int temp;
        int i;
        
        bzero(temp_path,100);
        
        if(glob(therm_pattern, 0, NULL, &therm_glob) != 0)
                return 0;

        for(i = 0; i < therm_glob.gl_pathc; i++){
                type_path = get_sysfs_string(therm_glob.gl_pathv[i]);
                if(strcmp(type_path, "x86_pkg_temp") == 0){
                        free(type_path);
                        break; /* found thermal zone for proc */
                }
                free(type_path);
        }

        /* i is the correct thermal zone number */
        snprintf(temp_path,100,"/sys/class/thermal/thermal_zone%d/temp",i);
        temp = get_sysfs_int(temp_path);
        free(temp_path);

        return temp/1000;
}





