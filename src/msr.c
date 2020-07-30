#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <glob.h>
#include <stdlib.h>

#include "msr.h"

int have_msr()
{
        return (access("/dev/cpu/0/msr", F_OK) != -1) ? 1 : 0;
}

uint64_t rdmsr_cpu(uint32_t reg, unsigned int cpu)
{
        int fd;
        uint64_t out = 0;
        char *path = NULL;
        if (asprintf(&path, "/dev/cpu/%d/msr", cpu) == -1)
                return 0;

        if ((fd = open(path, O_RDONLY)) == -1)
                return 0;

        if (pread(fd, &out, 8, (off_t) reg) != 8)
                out = 0;

        free(path);
        close(fd);
        return out;

}

uint64_t rdmsr(uint32_t reg)
{
        rdmsr_cpu(reg, 0);
}

uint64_t wrmsr_cpu(uint32_t reg, uint64_t val, unsigned int cpu)
{
        int fd;
        char *path = NULL;
        if (asprintf(&path, "/dev/cpu/%d/msr", cpu) == -1)
                return -1;

        if ((fd = open(path, O_WRONLY)) == -1)
                return -1;

        if (pwrite(fd, &val, 8, (off_t) reg) != 8){
                close(fd);
                perror(NULL);
                return -1;
        }
        free(path);
        close(fd);
        return 0;
       
}

uint64_t wrmsr(uint32_t reg, uint64_t val)
{
        return wrmsr_cpu(reg, val, 0);
}

uint64_t wrmsr_all(uint32_t reg, uint64_t val)
{
        glob_t globbuf;
        int cur_cpu;
        char *cpuglob = "/dev/cpu/*";
        
        glob(cpuglob, 0, NULL, &globbuf);
        
        if(globbuf.gl_pathc == 0){
                return -1; //wtf, we should have a single cpu at least
        }

        //iterate on glob results in case there's weirdness with onlined cpus
        for(int i = 0; i < globbuf.gl_pathc; i++){
                sscanf(globbuf.gl_pathv[i], "/dev/cpu/%d", &cur_cpu);
                wrmsr_cpu(reg, val, cur_cpu);
        }

        return 0;
}
