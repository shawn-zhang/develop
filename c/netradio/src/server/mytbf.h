#ifndef MYTBF_H
#define MYTBF_H

#define TBFMAX 1024

typedef void tbf_context_t;

extern	pthread_cond_t extern_cond;

tbf_context_t *tbf_ctx_new(int cps, int burst);

ssize_t tbf_write(int fd, const char *buf, size_t size, tbf_context_t*);

ssize_t tbf_read(int fd, char *buf, size_t size, tbf_context_t*);

int tbf_ctx_delete(tbf_context_t*);

#endif

