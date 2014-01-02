#ifndef _COMMAND_H_
#define _COMMAND_H_

#define CFG_MAXARGS 16

struct cmd_tbl_s {
	char	*name;		/* Command Name			*/
	int		maxargs;	/* maximum number of arguments	*/
	int		(*cmd)(struct cmd_tbl_s *,int,char *[]);
	char	*usage;		/* Usage message	(short)	*/
};

typedef struct cmd_tbl_s cmd_tbl_t;

extern cmd_tbl_t  __u_boot_cmd_start;
extern cmd_tbl_t  __u_boot_cmd_end;

typedef	void command_t (int, char *[]);

#define struct_section  __attribute__ ((unused,section (".u_boot_cmd")))

cmd_tbl_t *find_cmd(char *str);

//void do_nand_read(cmd_tbl_t *cmdp,int argc,char *argv[]);

#endif	/* __COMMAND_H */
