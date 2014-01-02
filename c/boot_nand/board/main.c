#include <common/common.h>
#include <driver/uart.h>
#include <driver/led.h>
#include <cpu/clock.h>
#include <driver/dram.h>
#include <driver/nand.h>
#include <common/command.h>
#include <lib/string.h>
#include <driver/dm9000.h>

#define BOOT_POS        TEXT_BASE
#define KERNEL          0x50008000
#define len             0x220000

int start_(void)
{
    clock_init();
    dram_init();
    nand_init();
    nand_read(BOOT_POS,0,0x40000);
    uart_init();
    led_on();

    return 0;
}

void do_command(char *command_buf)
{
    int argc;
    char *argv[CFG_MAXARGS + 1];
    cmd_tbl_t *cmdp;


    argc = parse_cmd(command_buf,argv);
    
    if (argc < 0){
        puts("Err: args is too many.\n");
        return ;
    }

    cmdp = find_cmd(argv[0]);
    if (cmdp == NULL){
        puts(argv[0]);
        puts(":no this cmd.\n");
        return;
    } 
    cmdp->cmd(cmdp,argc,argv);
    
    return ;
}

int main(void)
{
    char command_buf[COMMAND_BUFSIZE];
 
    eth_init();
    
    while(1){
        puts(PROMPT);
        gets(command_buf);
        putc('\r');
        if (strlen(command_buf)){
            do_command(command_buf);
        }
    }
    return 0;
}
