#ifndef _LINUX_STRING_H_
#define _LINUX_STRING_H_

#include <common/common.h>

extern char * ___strtok;
extern char * strpbrk(const char *,const char *);
extern char * strtok(char *,const char *);
extern char * strsep(char **,const char *);

extern char * strcpy(char *,const char *);
extern char * strncpy(char *,const char *,size_t);
extern char * strcat(char *, const char *);
extern char * strncat(char *, const char *,size_t);
extern int strcmp(const char *,const char *);
extern int strncmp(const char *,const char *,size_t);
extern char * strchr(const char *,int);
extern size_t strlen(const char *);
extern size_t strnlen(const char *,size_t);
extern char * strrchr(const char *,int);
extern char * strstr(const char *,const char *);
extern char * strdup(const char *);
extern char * strswab(const char *);
extern void * memset(void *,int,size_t);
extern void * memcpy(void *,const void *,size_t);
extern void * memmove(void *,const void *,size_t);
extern void * memscan(void *,int,size_t);
extern int memcmp(const void *,const void *,size_t);
extern void * memchr(const void *,int,size_t);

#endif /* _LINUX_STRING_H_ */
