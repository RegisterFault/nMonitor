#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "msr.h"

int have_msr()
{
        return (access("/dev/cpu/0/msr", F_OK) != -1) ? 1 : 0;
}

uint64_t rdmsr(uint32_t reg)
{
        int fd;
        unsigned long out = 0;
        if ((fd = open("/dev/cpu/0/msr", O_RDONLY)) == -1)
                return 0;

        if (lseek(fd, (off_t) reg, SEEK_SET) == -1)
                goto cleanup;
        
        if (read(fd, &out, 8) < 0)
                out = 0;

cleanup:
        close(fd);
        return out;
}


uint64_t  wrmsr(uint32_t reg, uint64_t val)
{
        int fd;
        if ((fd = open("/dev/cpu/0/msr", O_WRONLY)) == -1)
                return -1;

        if (pwrite(fd, &val, 8, reg) != 8){
                close(fd);
                perror(NULL);
                return -1;
        }
        close(fd);
        return 0;
}


