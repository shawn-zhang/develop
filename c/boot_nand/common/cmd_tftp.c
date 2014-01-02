#include <common/common.h>
#include <common/command.h>
#include <lib/string.h>
#include <net/net.h>


int do_tftp(cmd_tbl_t *cmdp,int argc,char *argv[])
{
    if (argc < 3){
        printf(cmdp->usage);
        return -1;
    }
    
    printf("do _tftp\n");
    tftp(argc,argv);
    
    return 0;
}

cmd_tbl_t cmd_tftp struct_section = {"tftp",3,do_tftp,"Usage: tftp <mem_addr> <filename>.\n\0"};
