#ifndef CONF_H
#define CONF_H

#define DEFAULT_MEDIA_PATH "/var/lib/media"
#define DEFAULT_ETH	"eth0"

struct conf_st {
	char *mgroup;
	char *port;
	char *media_path;
	char *eth;
	char debug;
};

extern struct conf_st global_conf;

#endif

