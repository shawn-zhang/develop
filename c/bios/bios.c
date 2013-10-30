#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>

#define DEV_NAME        "/dev/mem"

#define BIOS_VIDIOROM_START  0xc0000
#define BIOS_VIDIOROM_SIZE   0x40000

#define BIOS_RAM_START  0x400
#define BIOS_RAM_SIZE   0x100

int read_mmap(char *name, unsigned char *buf, off_t start, unsigned size)
{
    off_t map_start, xofs;
    int psize = getpagesize(), fd;
    unsigned map_size;
    void *p;
    struct stat sbuf;

    if(!size || !name) return 0;

    memset(buf, 0, size);

    map_start = start & -psize;
    xofs = start - map_start;

    map_size = (xofs + size + psize - 1) & -psize;
    
    fd = open(name, O_RDONLY);

    if(fd == -1) return 0;

    if(!fstat(fd, &sbuf) && S_ISREG(sbuf.st_mode)) {
        if(sbuf.st_size < start + size) {
            if(sbuf.st_size > start) {
                size = sbuf.st_size - start;
            }
            else {
                size = 0;
            }
        }
    }

    if(!size) {
        close(fd);
        return 0;
    }

    p = mmap(NULL, map_size, PROT_READ, MAP_PRIVATE, fd, map_start);
    if(p == MAP_FAILED) {
        printf("%s[0x%x, %u]: mmap(, %u,,,, 0x%x) failed: %s\n",name, (unsigned) start, size, map_size, (unsigned) map_start, strerror(errno));
        close(fd);
        return 0;
    }
    printf("%s[0x%x, %u]: mmap(, %u,,,, 0x%x) ok\n",name, (unsigned) start, size, map_size, (unsigned) map_start);
    memcpy(buf, p + xofs, size);
    munmap(p, map_size);
    close(fd);
    
    return 1;
}

/**
 *
 *  关于BIOS_RAM及BIOS_ROM内存内容的解析，详见hwinfo代码的,bios.c->hd_scan_bios函数
 *
 *
 */


int main(int argc, char *argv)
{   
    int i = 0;
    unsigned char bsrom_buf[BIOS_ROM_SIZE] = {0};
    unsigned char bsram_buf[BIOS_RAM_SIZE] = {0};

    read_mmap(DEV_NAME, bsrom_buf, BIOS_ROM_START, BIOS_ROM_SIZE);
    read_mmap(DEV_NAME, bsram_buf, BIOS_RAM_START, BIOS_RAM_SIZE);
    
    printf("BIOS ROM DATA:");
    for(i = 0; i < BIOS_ROM_SIZE; i++){
        if ((i % 32) == 0){
            printf("\n");
        }
        printf(" %02x",bsrom_buf[i]);
    }
    printf("\n");
    printf("BIOS RAM DATA:");
    for(i = 0; i < BIOS_RAM_SIZE; i++){
        if ((i % 32) == 0){
            printf("\n");
        }
        printf(" %02x",bsram_buf[i]);
    }
    printf("\n");

    return 0;
}
