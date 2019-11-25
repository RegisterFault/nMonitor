#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "msr.h"

void fail()
{
        perror(NULL);
        exit(1);
}


unsigned long rdmsr(unsigned int reg)
{
        int fd;
        unsigned long out;
        if ((fd = open("/dev/cpu/0/msr", O_RDONLY)) == -1)
                fail();

        if (lseek(fd, (off_t) reg, SEEK_SET) == -1)
                fail();
        
        if (read(fd, &out, 8) == -1){
                close(fd);
                return -1;
        }

        close(fd);

        return out;
}


unsigned long wrmsr(unsigned int reg, unsigned long val)
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


