#ifndef STRING_H
#define STRING_H

#include "at_types.h"


extern void *memcpy(void *dst, const void *src, size_t count);
extern void *memcpy_32(void *dst, const void *src, size_t count);
extern void *memmove(void *dst, const void *src, size_t count);
extern char *strcpy(char *dst, const char *src);
extern char *strncpy(char *dst, const char *src, size_t count);
extern char *strcat(char *dst, const char *src);
extern char *strncat(char *dst, const char *src, size_t count);
extern int memcmp(const void *cs, const void *ct, size_t count);
extern int strcmp(const char *cs, const char *ct);
extern int strncmp(const char *cs, const char *ct, size_t count);
extern void *memchr(const void *s, int c, size_t n);
extern char *strchr(const char *s, int c);
extern size_t strcspn(const char *s, const char *reject);
extern char *strpbrk(const char *cs, const char *ct);
extern char *strrchr(const char *s, int c);
extern size_t strspn(const char *s, const char *accept);
extern char *strstr(const char *s1, const char *s2);
extern void *memset(void *s, int c, size_t count);
extern size_t strlen(const char *s);
extern int strnicmp(const char *s1, const char *s2, size_t len);
extern int strcasecmp(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, size_t n);
extern size_t strlcpy(char *dest, const char *src, size_t size);
extern size_t strlcat(char *dest, const char *src, size_t count);
extern char *strnchr(const char *s, size_t count, int c);
extern char *skip_spaces(const char *str);
extern char *strim(char *s);
extern char *strsep(char **s, const char *ct);
extern void *memscan(void *addr, int c, size_t size);
extern char *strnstr(const char *s1, const char *s2, size_t len);
extern size_t strnlen(const char *s, size_t count);
extern int atoi(const char *s);
extern int str2num(char *s);


#endif /* STRING_H */

