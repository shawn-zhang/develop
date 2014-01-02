#include <common/common.h>
#include <common/command.h>
#include <lib/string.h>

cmd_tbl_t *find_cmd(char *str)
{
    cmd_tbl_t *cur;
    cmd_tbl_t *end = &__u_boot_cmd_end; 

    for(cur = (void *)(&__u_boot_cmd_start); (u32 )cur < (u32 )end; cur++){
        if (!strcmp(str,cur->name)){
            return cur;
        }
    }

    return NULL;

}
