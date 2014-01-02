#include <common/common.h>
#include <common/command.h>
#include <driver/nand.h>
#include <driver/uart.h>


int do_nand_read(cmd_tbl_t *cmdp,int argc,char *argv[])
{
    if (argc < cmdp->maxargs){
        puts(cmdp->usage);
    }

    nand_read(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]));
    puts("nandread ok.\n");
    return 0;
}

cmd_tbl_t cmd_nand_read struct_section = {"nandread",4,do_nand_read,"Usage: nandread <mem_addr> <nand_addr> <len>.\n\0"};

int do_nand_write(cmd_tbl_t *cmdp,int argc,char *argv[])
{
    if (argc < cmdp->maxargs){
        puts(cmdp->usage);
    }
    nand_write(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]));

    return 0;
}

cmd_tbl_t cmd_nand_write struct_section = {"nandwrite",4,do_nand_write,"Usage: nandwrite <mem_addr> <nand_addr> len.\n\0"};

int do_nand_erase(cmd_tbl_t *cmdp,int argc,char *argv[])
{
    if (argc < cmdp->maxargs){
        puts(cmdp->usage);
    }
    nand_erase(atoi(argv[1]),atoi(argv[2]));

    return 0;
}

cmd_tbl_t cmd_nand_erase struct_section = {"nanderase",3,do_nand_erase,"Usage: nanderase <nand_addr> <len>.\n\0"};

