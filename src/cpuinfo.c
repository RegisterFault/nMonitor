#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cpuinfo.h"

long int get_sysfs_int(char *path)
{
        FILE * f = fopen(path,"r");
        long int out;
        if(!f)
                return 0;
        if(fscanf(f, "%lld", &out) != 1)
                return 0;
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
        char *token;

        
        while(getline(&lbuf, (size_t *)&size, f)){
                if(strstr(lbuf,"model name")){
                        strtok(lbuf," ");
                        strtok(NULL," ");
                        strtok(NULL," ");
                        strtok(NULL," ");
                        token = strtok(NULL," ");
                        printf("%s\n",token);
                        break;
                }
        }

        strncpy(*str, token, in_size);
        free(lbuf);
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
        char * token;
        in->total = 0;
        in->free = 0;
        in->avail = 0;
        in->cache = 0;

        while(getline(&lbuf,(size_t *)&size, f)){
                if(strstr(lbuf,"MemTotal:")) {
                        strtok(lbuf," ");
                        token = strtok(NULL," ");
                        sscanf(token,"%lld",&(in->total));
                }
                if(strstr(lbuf,"MemFree:")) {
                        strtok(lbuf, " ");
                        token = strtok(NULL, " ");
                        sscanf(token, "%lld", &(in->free));
                } 
                if(strstr(lbuf,"MemAvailable:")) {
                        strtok(lbuf, " ");
                        token = strtok(NULL, " ");
                        sscanf(token, "%lld", &(in->avail));
                } 
                if(strstr(lbuf,"Cached:")) {
                        strtok(lbuf, " ");
                        token = strtok(NULL, " ");
                        sscanf(token, "%lld", &(in->cache));
                } 
                if(in->free && in->total && in->avail && in->cache)
                        break;
        }
}
