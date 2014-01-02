#ifndef _TFTP_H
#define	_TFTP_H

/*
 * Trivial File Transfer Protocol (IEN-133)
 */
#define	SEGSIZE		512		/* data segment size */

#define netascii	"netascii\0"
#define octet		"octet\0"
#define mail		"mail\0"
#define TIMEOUT		(1000 * 1000 * 1000)
#define DEFAULTPORT	69


#define min(a,b)\
	((a) > (b) ? (b) : (a))


/*
 * Packet types.
 */
enum __opcode{
	TFTP_RRQ = 1,				/* read request */
	TFTP_WRQ,				/* write request */
	TFTP_DATA,				/* data packet */
	TFTP_ACK,				/* acknowledgement */
	TFTP_ERROR				/* error code */
};
struct	tftphdr {
	short	th_opcode;			/* packet type */
	union {
		unsigned short	tu_block;	/* block # */
		short	tu_code;		/* error code */
		char	tu_stuff[1];		/* request packet stuff */
	} __attribute__ ((__packed__)) th_u;
	char	th_data[1];			/* data or error string */
} __attribute__ ((__packed__));

typedef struct tftphdr  tftp_t;

#define	th_block	th_u.tu_block
#define	th_code		th_u.tu_code
#define	th_stuff	th_u.tu_stuff
#define	th_msg		th_data

/*
 * Error codes.
 */
enum __errno{	
	EUNDEF,		/* not defined */
	ENOTFOUND,	/* file not found */
	EACCESS,	/* access violation */
	ENOSPACE,	/* disk full or allocation exceeded */
	EBADOP,		/* illegal TFTP operation */
	EBADID,		/* unknown transfer ID */
	EEXISTS,	/* file already exists */
	ENOUSER		/* no such user */

};

void tftp_unpack();

extern int send_rrq(char *);

extern int send_ack(short);

extern int recv(tftp_t *,int);


#endif /*tftp.h */
