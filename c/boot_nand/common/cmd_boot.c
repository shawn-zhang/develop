#include <common/common.h>
#include <common/command.h>
#include <lib/setup.h>
#include <lib/string.h>
#include <driver/nand.h>

#define TAGLISTADDR     0x50000100
#define KENELADDR       0x50008000

int do_boot(cmd_tbl_t *cmdp,int argc,char *argv[])
{
    //char *cmdline = "root=/dev/mtdblock2 rw ip=192.168.1.10 console=ttySAC0,115200 mem=128M"; 
    char *cmdline = "root=/dev/nfs rw nfsroot=192.168.1.156:/nfs_root ip=192.168.1.10 console=ttySAC0,115200 mem=128M"; 
    void (* kernel)(int,int,void *) = (void *)KENELADDR;
    u32 *taglist = (void *)TAGLISTADDR;

    *taglist = 2;
    *(taglist + 1) = ATAG_CORE;

    taglist += *taglist;
    *taglist = (strlen(cmdline) + 1 + 3 + sizeof(struct tag_header)) >> 2;
    *(taglist + 1) = ATAG_CMDLINE;
    memcpy((void *)(taglist + 2),(void *)cmdline,strlen(cmdline) + 1);

    taglist += *taglist;
    *taglist = 0;
    *(taglist + 1) = ATAG_NONE;
    
    kernel(0,1626,(void *)taglist);

    return 0;
}

cmd_tbl_t cmd_bootm struct_section = {"bootm",2,do_boot,"Usage: bootm <mem_addr> or bootm .\n\0"};
