#ifndef CONF_H
#define CONF_H

#define DEFAULT_ETH	"eth0"
struct conf_st {
	char *mgroup;
	char *port;
	char *player_cmdl;
	char *eth;
};

extern struct conf_st global_conf;

#endif

