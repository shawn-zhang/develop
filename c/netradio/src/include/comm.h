#ifndef COMM_H
#define COMM_H

#include <stdint.h>

#define DEFAULT_MGROUP	"224.2.2.2"
#define DEFAULT_PORT	"1984"
#define DEFAULT_CPS	(8 * 1024)
#define DEFAULT_BUST	(64 * 1024)

#define CHNID_MIN	0
#define CHNID_MAX	199
#define CHNID_LIST	255
#define ENTRY_SIZE	(sizeof(struct msg_chn_entry_st) - sizeof(uint8_t))


typedef uint8_t chn_id_t;

enum{
	ST_ON = 1,
	ST_OFF
};


struct msg_data_st {
	chn_id_t chn_id;	/* Always >=CHNID_MIN && <=CHNID_MAX */
	uint8_t data[1];	/* */
};

struct msg_chn_entry_st {
	chn_id_t chn_id;
	uint16_t len;
	uint8_t status;
	uint8_t descript[1];
}__attribute__((packed));

struct msg_chn_list_st {
	chn_id_t chn_id;	/* Always CHNID_LIST */
	uint8_t chn_nr;
	struct msg_chn_entry_st entry[1];	/*All channel entry*/
}__attribute__((packed));

#endif

