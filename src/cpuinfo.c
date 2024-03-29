#define _GNU_SOURCE
#include <fcntl.h>
#include <glob.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cpuinfo.h"
#include "msr.h"
#include "modelparse.h"

/* arbitrary max frequency bounds to use when max freq of processor unfetchable */
#define MAX_FREQ_ARB 5000

char * BatteryPath = NULL; //populated by init_batinfo
char * PowerPath = NULL;   //populated by init_powerinfo

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
                /* BatteryPath only gets allocated once, and should never be freed*/
                BatteryPath = calloc(len, 1);
                /* glob sorts results, so we get the lowest-numbered BAT entry */
                strcpy(BatteryPath, globbuf.gl_pathv[0]); 
        }

        globfree(&globbuf);
        return;       
}

void init_powerinfo()
{
        int i;
        static char *power_paths[] = {
                "/sys/class/power_supply/AC",
                "/sys/class/power_supply/AC0",
                "/sys/class/power_supply/ADP0",
                "/sys/class/power_supply/ACAD",
                NULL
        };

        for (i = 0; power_paths[i] != NULL; i++)
                if(access(power_paths[i], F_OK) == 0)
                        break;

        PowerPath = power_paths[i];
}

long int get_sysfs_int(char *path)
{
        FILE *f = fopen(path,"r");
        long int out = 0;
        if (!f)
                return out;
        
        fscanf(f, "%ld", &out);
        
        fclose(f);
        return out;
}

/* always make sure to free the results of this function */
char *get_sysfs_string(const char *path)
{
        FILE *f = fopen(path, "r");
        char *out = NULL;
        if (!f)
                return NULL;;
        /* reminder that %m means scanf will malloc */
        fscanf(f, "%ms", &out);
        
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

int extract_file_int(char *path, char *search, char* template)
{
        FILE *f = fopen(path, "r");
        size_t size = 1024;
        char *lbuf;
        int out = 0;

        if(!f)
                return 0;

        lbuf = malloc(size);

        while (getline(&lbuf, &size, f) != -1)
                if (strstr(lbuf, search))
                        break;
        sscanf(lbuf, template, &out);
        
        fclose(f);
        free(lbuf);
        return out;
}

int get_cpu_family()
{
        char *path = "/proc/cpuinfo";
        char *search = "cpu family";
        char *template = "cpu family\t: %d";
        return extract_file_int(path, search, template);
}

int get_cpu_model()
{
        char *path = "/proc/cpuinfo";
        char *search = "model\t\t";
        char *template = "model\t\t: %d";
        return extract_file_int(path, search, template);
}

void get_amd_cpuname(char **str)
{
    *str = fetch_amd_cpu_model();
}

/* mallocs output string, free after use */
void get_intel_cpuname(char **str)
{
    *str = fetch_intel_cpu_model();
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

int ac_present()
{
        return (PowerPath != NULL) ? 1 : 0;
}


int ac_attached()
{
        char *path = NULL;
        int ret;

        if (!ac_present())
                return 0;

        asprintf(&path, "%s/online", PowerPath);
        ret = get_sysfs_int(path);

        free(path);
        return ret;
}

int is_charging()
{
        char *path = NULL;
        char *status = NULL;
        int ret = 0;
        asprintf(&path, "%s/status", BatteryPath);

        status = get_sysfs_string(path);
        free(path);
        if(status == NULL)
                return 0;

        if(strcmp(status, "Charging") == 0)
                ret = 1;

        free(status);
        return ret;
}

/* some systems report current charge, some report wattage */
int is_current()
{
        char *path = NULL;
        int ret;

        if(!BatteryPath)
                return 0;
      
        asprintf(&path,"%s/charge_full", BatteryPath);
        ret = (access(path, F_OK) == 0) ? 1 : 0;
        free(path);
        
        return ret;
}

int get_charge_pct()
{
        char *full = NULL;
        char *now = NULL;
        
        if(!BatteryPath)
                return 0;
        
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
        
        if(!BatteryPath)
                return 0.0;

        asprintf(&path, "%s/charge_full", BatteryPath);
        
        float ret = ((float) get_sysfs_int(path)) / 1000000.0;
        
        free(path);
        return ret;
}

float get_charge_full_design()
{
        char *path = NULL;
        
        if(!BatteryPath)
                return 0.0;

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
        
        if(!BatteryPath)
                return 0;

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
        
        if(!BatteryPath)
                return 0;

        asprintf(&path, "%s/power_now", BatteryPath);
        
        long int ret = get_sysfs_int(path) / 1000;

        free(path);
        return ret;
}

long int get_freq()
{
        return get_cur_freq(0);
}

long int get_cur_freq(int cpu)
{
        char *path = NULL;
        long int ret;
        asprintf(&path, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", cpu);
        ret = get_sysfs_int(path) / 1000;
        free(path);
        return ret;
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

        if(!BatteryPath)
                return 0;

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
        
        if(!BatteryPath)
                return 0;

        asprintf(&path, "%s/energy_full", BatteryPath);
        
        int ret = get_sysfs_int(path) / 1000000;

        free(path);
        return ret;
}

int get_bat_design()
{
        char *path = NULL;
        
        if(!BatteryPath)
                return 0;

        asprintf(&path, "%s/energy_full_design", BatteryPath);
        int ret = get_sysfs_int(path) / 1000000;

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

int get_sockets()
{
        char *path = "/proc/cpuinfo";
        FILE *f = fopen(path,"r");
        size_t size = 1024;
        char *lbuf = malloc(size);
	int phys_id = 0;
	int highest_phys_id = 0;

        while (getline(&lbuf, &size, f) != -1){
		if (strstr(lbuf, "physical id")) {
			sscanf(lbuf, "physical id\t: %d", &phys_id);
			if (phys_id > highest_phys_id){
				highest_phys_id = phys_id;
			}
		}
	}

        free(lbuf);
        fclose(f);
	return highest_phys_id + 1; //physical ids are zero-indexed
}

int get_cores()
{
        char *path = "/proc/cpuinfo";
        FILE *f = fopen(path,"r");
        size_t size = 1024;
        char *lbuf = malloc(size);
        int pkg_cores = 0;
	int total_cores = 0;
	int sockets = get_sockets();
        if (!f)
                return 0;

	/* assuming multi-socket x64 systems are symmetrical */
        while (getline(&lbuf, &size, f) != -1)
                if(strstr(lbuf, "cpu cores"))
			break;
	sscanf(lbuf, "cpu cores\t: %d", &pkg_cores);

	total_cores = sockets * pkg_cores;

        fclose(f);
        free(lbuf);
        return total_cores;
}

int get_threads()
{
        char *path = "/proc/cpuinfo";
        FILE *f = fopen(path,"r");
        size_t size = 1024;
        char *lbuf = malloc(size);
        int threads = 0;
	int siblings = 0;
	int sockets = get_sockets();
        if (!f)
                return 0;

	/* assuming multi-socket x64 systems are symmetric */
        while (getline(&lbuf, &size, f) != -1)
                if (strstr(lbuf, "siblings"))
			break;
	sscanf(lbuf, "siblings\t: %d", &siblings);

	threads = sockets * siblings;

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
        char *temp_path;
        int temp = 0;
        int i; //iterator preserved for use as hwmon index

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
        asprintf(&temp_path, "/sys/class/hwmon/hwmon%d/temp1_input", i);
        temp = get_sysfs_int(temp_path)/1000;
        free(temp_path);

cleanup:
        globfree(&therm_glob);
        return temp;
}

int get_intel_temp()
{
        /* find x86_pkg_temp */
        char *therm_pattern = "/sys/class/thermal/thermal_zone*/type";
        glob_t therm_glob;
        char *type_path;
        char *temp_path;
        int temp = 0;
        int i; // iterator preserved to locate the correct thermal_zone
        
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
        asprintf(&temp_path, "/sys/class/thermal/thermal_zone%d/temp", i);
        temp = get_sysfs_int(temp_path) / 1000;
        free(temp_path);

cleanup:
        globfree(&therm_glob);
        return temp;
}

/* requires root */
char get_throttle_char()
{
        CORE_PERF_LIMIT_REASONS a;
        a.w = rdmsr(CORE_PERF_LIMIT_REASONS_MSR);

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
double get_pkg_joules()
{
        return is_amd() ? get_amd_pkg_joules() : get_intel_pkg_joules();
}

/* requires root */
double get_intel_pkg_joules()
{
        RAPL_POWER_UNIT u;
        PKG_ENERGY_STATUS e;

        u.w = rdmsr(RAPL_POWER_UNIT_MSR);
        e.w = rdmsr(PKG_ENERGY_STATUS_MSR);

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
        RAPL_POWER_UNIT u;
        PP0_ENERGY_STATUS e;

        u.w = rdmsr(RAPL_POWER_UNIT_MSR);
        e.w = rdmsr(PP0_ENERGY_STATUS_MSR);

        return (double) (e.s.energy * (pow(0.5, (double) u.s.es_units)));
}

/* requires root */
double get_pp1_joules()
{ 
        RAPL_POWER_UNIT u;
        PP1_ENERGY_STATUS e;

        u.w = rdmsr(RAPL_POWER_UNIT_MSR);
        e.w = rdmsr(PP1_ENERGY_STATUS_MSR);

        return (double) (e.s.energy * (pow(0.5, (double) u.s.es_units)));
}

/* requires root */
double get_dram_joules()
{
        RAPL_POWER_UNIT u;
        DRAM_ENERGY_STATUS  e;

        u.w = rdmsr(RAPL_POWER_UNIT_MSR);
        e.w = rdmsr(DRAM_ENERGY_STATUS_MSR);

        return (double) (e.s.energy * (pow(0.5, (double) u.s.es_units)));
}

/* requires root */
double get_volt( unsigned int plane)
{
        /* 
         * kernel lockdown can forbid the use of this undocumented MSR and flood dmesg
         * We should have a flag for determining if we still try to pull from this,
         * toggling to false if the first attempt to read the msr failed
         */
        static bool can_get_volt = true; 
        if(!can_get_volt)
                return 0.0;
        VOLT v;
        if (plane > 4)
                return 0.0;

        /* have to write which plane to read */
        v.w = VOLT_READ;
        v.s.plane = plane;
        if (wrmsr(VOLT_MSR, v.w) == -1){
                can_get_volt = false;
                return 0.0;
        }

        v.w = rdmsr(VOLT_MSR);
        return v.s.volt / 1.024;
}       

int hwp_enabled()
{
        IA32_PM_ENABLE pmemsr;
        pmemsr.w = rdmsr(IA32_PM_ENABLE_MSR);
        if(pmemsr.s.enable == 1);
                return 1;
        return 0;
}

int get_hwp_pref()
{
        //sometimes this is controlled at the package level, but intel_pstate likes to do per-cpu
        IA32_HWP_REQUEST_PKG pkg_rq;
        IA32_HWP_REQUEST rq;

        pkg_rq.w = rdmsr(IA32_HWP_REQUEST_PKG_MSR);
        rq.w = rdmsr(IA32_HWP_REQUEST_MSR);

        if (rq.s.epp_valid)
                return rq.s.nrg_pref;
        else
                return pkg_rq.s.nrg_pref;
}
