#ifndef MEDIALIB_H
#define MEDIALIB_H

#include <comm.h>

int media_get_channel_list(struct msg_chn_entry_st **, int *chn_nr);

void media_free_channel_list(struct msg_chn_entry_st *, int chn_nr);

void media_free_channel_entry(struct msg_chn_entry_st *,chn_id_t chn_id);

ssize_t media_read(chn_id_t, char *buf, size_t len);

#endif

