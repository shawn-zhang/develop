#include <common/common.h>
#include <common/command.h>
#include <lib/string.h>


int do_go(cmd_tbl_t *cmdp,int argc,char *argv[])
{
    if (argc < 2){
        printf(cmdp->usage);
        return -1;
    }
    
    int (*func)(int,char *[]) = (void *)(atoi(argv[1]));
    
    func(argc - 1,argv + 1);
    
    return 0;
}

cmd_tbl_t cmd_go struct_section = {"go",3,do_go,"Usage: go <mem_addr> <args> [...].\n\0"};
