#ifndef THR_CHANNEL_H
#define THR_CHANNEL_H

int thr_channel_create(struct msg_chn_entry_st *, int len);

int thr_channel_destroy(struct msg_chn_entry_st *,chn_id_t);

int thr_channel_destroy_all(struct msg_chn_entry_st *,int chn_nr);

#endif

