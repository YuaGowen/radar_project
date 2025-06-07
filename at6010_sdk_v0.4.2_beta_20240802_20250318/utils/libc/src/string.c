#include "ctype.h"
#include "string.h"
#include "at_assert.h"

/**
 * memcpy - Copy one area of memory to another
 * \param    dst: Where to copy to
 * \param    src: Where to copy from
 * \param    count: The size of the area.
 */
void *memcpy(void *dst, const void *src, size_t count)
{
    char *tmp = dst;
    const char *s = src;
    while(count--)
    {
        *tmp++ = *s++;
    }
    return dst;
}

/**
 * memcpy_32 - Copy one area of memory to another in 32bit
 * \param    dst: Where to copy to
 * \param    src: Where to copy from
 * \param    count: The size of the area.
 */
void *memcpy_32(void *dst, const void *src, size_t count)
{
    if(is_n_aligned((u32)dst, 4) && is_n_aligned((u32)src, 4))
    {
        u32 *tmp = dst;
        const u32 *s = src;
        u32 count_32  = (count >> 2);
        u8 left = count - (count_32 << 2);

        while(count_32--)
        {
            *tmp++ = *s++;
        }

        if(left != 0)
        {
            char *tmp = dst;
            const char *s = src;
            while(left--)
            {
                *tmp++ = *s++;
            }
        }
    }
    else
    {
        char *tmp = dst;
        const char *s = src;
        while(count--)
        {
            *tmp++ = *s++;
        }
    }

    return dst;
}

/**
 * memmove - Copy one area of memory to another
 * \param    dest: Where to copy to
 * \param    src: Where to copy from
 * \param    count: The size of the area.
 *
 * Unlike memcpy(), memmove() copes with overlapping areas.
 */
void *memmove(void *dst, const void *src, size_t count)
{
    char *tmp;
    const char *s;

    if(dst <= src)
    {
        tmp = dst;
        s = src;
        while(count--)
        {
            *tmp++ = *s++;
        }
    }
    else
    {
        tmp = dst;
        tmp += count;
        s = src;
        s += count;
        while(count--)
        {
            *--tmp = *--s;
        }
    }

    return dst;
}

/**
 * strcpy - Copy a %NUL terminated string
 * \param    dst: Where to copy the string to
 * \param    src: Where to copy the string from
 */
char *strcpy(char *dst, const char *src)
{
    char *tmp = dst;

    while((*dst++ = *src++) != '\0')
        /* nothing */;

    return tmp;
}

/**
 * strncpy - Copy a length-limited, %NUL-terminated string
 * \param    dst: Where to copy the string to
 * \param    src: Where to copy the string from
 * \param    count: The maximum number of bytes to copy
 *
 * The result is not %NUL-terminated if the source exceeds
 * count bytes.
 *
 * In the case where the length of src is less than that of
 * count, the remainder of dest will be padded with %NUL.
 *
 */
char *strncpy(char *dst, const char *src, size_t count)
{
    char *tmp = dst;

    while(count)
    {
        if((*tmp = *src) != 0)
        {
            src++;
        }

        tmp++;
        count--;
    }

    return dst;
}

/**
 * strcat - Append one %NUL-terminated string to another
 * \param    dst: The string to be appended to
 * \param    src: The string to append to it
 */
char *strcat(char *dst, const char *src)
{
    char *tmp = dst;

    while(*dst)
    {
        dst++;
    }

    while((*dst++ = *src++) != '\0')
        /* nothing */;

    return tmp;
}

/**
 * strncat - Append a length-limited, %NUL-terminated string to another
 * \param    dst: The string to be appended to
 * \param    src: The string to append to it
 * \param    count: The maximum numbers of bytes to copy
 *
 * Note that in contrast to strncpy(), strncat() ensures the result is
 * terminated.
 */
char *strncat(char *dst, const char *src, size_t count)
{
    char *tmp = dst;

    if(count)
    {
        while(*dst)
        {
            dst++;
        }

        while((*dst++ = *src++) != 0)
        {
            if(--count == 0)
            {
                *dst = '\0';
                break;
            }
        }
    }

    return tmp;
}

/**
 * memcmp - Compare two areas of memory
 * \param    cs: One area of memory
 * \param    ct: Another area of memory
 * \param    count: The size of the area.
 */
int memcmp(const void *cs, const void *ct, size_t count)
{
    const unsigned char *su1, *su2;
    int res = 0;

    for(su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
    {
        if((res = *su1 - *su2) != 0)
        {
            break;
        }
    }
    return res;
}

/**
 * strcmp - Compare two strings
 * \param    cs: One string
 * \param    ct: Another string
 */
int strcmp(const char *cs, const char *ct)
{
    unsigned char c1, c2;

    while(1)
    {
        c1 = *cs++;
        c2 = *ct++;
        if(c1 != c2)
        {
            return ((c1 < c2) ? -1 : 1);
        }

        if(!c1)
        {
            break;
        }
    }

    return 0;
}

/**
 * strncmp - Compare two length-limited strings
 * \param    cs: One string
 * \param    ct: Another string
 * \param    count: The maximum number of bytes to compare
 */
int strncmp(const char *cs, const char *ct, size_t count)
{
    unsigned char c1, c2;

    while(count)
    {
        c1 = *cs++;
        c2 = *ct++;
        if(c1 != c2)
        {
            return ((c1 < c2) ? -1 : 1);
        }
        if(!c1)
        {
            break;
        }
        count--;
    }

    return 0;
}

/**
 * memchr - Find a character in an area of memory.
 * \param    s: The memory area
 * \param    c: The byte to search for
 * \param    n: The size of the area.
 *
 * returns the address of the first occurrence of \param    c, or %NULL
 * if c is not found
 */
void *memchr(const void *s, int c, size_t n)
{
    const unsigned char *p = s;
    while(n-- != 0)
    {
        if(c == *p++)
        {
            return (void *)(p - 1);
        }
    }

    return NULL;
}

/**
 * strchr - Find the first occurrence of a character in a string
 * \param    s: The string to be searched
 * \param    c: The character to search for
 */
char *strchr(const char *s, int c)
{
    for(; *s != c; ++s)
    {
        if(*s == '\0')
        {
            return NULL;
        }
    }

    return (char *)s;
}

/**
 * strcspn - Calculate the length of the initial substring of \param    s which does
 *           not contain letters in \param    reject
 * \param    s: The string to be searched
 * \param    reject: The string to avoid
 */
size_t strcspn(const char *s, const char *reject)
{
    const char *p;
    const char *r;
    size_t count = 0;

    for(p = s; *p != '\0'; ++p)
    {
        for(r = reject; *r != '\0'; ++r)
        {
            if(*p == *r)
            return count;
        }
        ++count;
    }

    return count;
}

/**
 * strpbrk - Find the first occurrence of a set of characters
 * \param    cs: The string to be searched
 * \param    ct: The characters to search for
 */
char *strpbrk(const char *cs, const char *ct)
{
    const char *sc1, *sc2;

    for(sc1 = cs; *sc1 != '\0'; ++sc1)
    {
        for(sc2 = ct; *sc2 != '\0'; ++sc2)
        {
            if(*sc1 == *sc2)
            {
                return (char *)sc1;
            }
        }
    }

    return NULL;
}

/**
 * strrchr - Find the last occurrence of a character in a string
 * \param    s: The string to be searched
 * \param    c: The character to search for
 */
char *strrchr(const char *s, int c)
{
    const char *p = s + strlen(s);
    do{
        if(*p == c)
        {
            return (char *)p;
        }
    } while(--p >= s);

    return NULL;
}

/**
 * strspn - Calculate the length of the initial substring of \param    s which only
 *          contain letters in \param    accept
 * \param    s: The string to be searched
 * \param    accept: The string to search for
 */
size_t strspn(const char *s, const char *accept)
{
    const char *p;
    const char *a;
    size_t count = 0;

    for(p = s; *p != '\0'; ++p)
    {
        for(a = accept; *a != '\0'; ++a)
        {
            if(*p == *a)
            {
                break;
            }
        }

        if(*a == '\0')
        {
            return count;
        }

        ++count;
    }

    return count;
}

/**
 * strstr - Find the first substring in a %NUL terminated string
 * \param    s1: The string to be searched
 * \param    s2: The string to search for
 */
char *strstr(const char *s1, const char *s2)
{
    size_t l1, l2;

    l2 = strlen(s2);
    if(!l2)
    {
        return (char *)s1;
    }
    l1 = strlen(s1);
    while(l1 >= l2)
    {
        l1--;
        if(!memcmp(s1, s2, l2))
        {
            return (char *)s1;
        }
        s1++;
    }

    return NULL;
}

/**
 * memset - Fill a region of memory with the given value
 * \param    s: Pointer to the start of the area.
 * \param    c: The byte to fill the area with
 * \param    count: The size of the area.
 */
void *memset(void *s, int c, size_t count)
{
    u8 *xs = s;

    while(count--)
    {
        *xs++ = c;
    }

    return s;
}

/**
 * strlen - Find the length of a string
 * \param    s: The string to be sized
 */
size_t strlen(const char *s)
{
    const char *sc;

    for(sc = s; *sc != '\0'; ++sc)
        /* nothing */;
    return sc - s;
}

/**
 * strnicmp - Case insensitive, length-limited string comparison
 * \param    s1: One string
 * \param    s2: The other string
 * \param    len: the maximum number of characters to compare
 */
int strnicmp(const char *s1, const char *s2, size_t len)
{
    /* Yes, Virginia, it had better be unsigned */
    unsigned char c1, c2;

    if(!len)
    {
        return 0;
    }

    do{
        c1 = *s1++;
        c2 = *s2++;
        if(!c1 || !c2)
        {
            break;
        }

        if(c1 == c2)
        {
            continue;
        }

        c1 = tolower(c1);
        c2 = tolower(c2);
        if(c1 != c2)
        {
            break;
        }
    } while(--len);

    return (int)c1 - (int)c2;
}

int strcasecmp(const char *s1, const char *s2)
{
    int c1, c2;

    do{
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
    } while((c1 == c2) && (c1 != 0));

    return c1 - c2;
}

/* Compare string case non-sensitive */
int strncasecmp(const char *s1, const char *s2, size_t n)
{
    char c1, c2;

    do
    {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);

    } while((--n > 0) && (c1 == c2) && (c1 != '\0')) ;

    return (c1 - c2);
}

/**
 * strlcpy - Copy a %NUL terminated string into a sized buffer
 * \param    dest: Where to copy the string to
 * \param    src: Where to copy the string from
 * \param    size: size of destination buffer
 *
 * Compatible with *BSD: the result is always a valid
 * NUL-terminated string that fits in the buffer (unless,
 * of course, the buffer size is zero). It does not pad
 * out the result like strncpy() does.
 */
size_t strlcpy(char *dest, const char *src, size_t size)
{
    size_t ret = strlen(src);

    if(size)
    {
        size_t len = (ret >= size) ? size - 1 : ret;
        memcpy(dest, src, len);
        dest[len] = '\0';
    }

    return ret;
}

/**
 * strlcat - Append a length-limited, %NUL-terminated string to another
 * \param    dest: The string to be appended to
 * \param    src: The string to append to it
 * \param    count: The size of the destination buffer.
 */
size_t strlcat(char *dest, const char *src, size_t count)
{
    size_t dsize = strlen(dest);
    size_t len = strlen(src);
    size_t res = dsize + len;

    dest += dsize;
    count -= dsize;
    if(len >= count)
    {
        len = count-1;
    }

    memcpy(dest, src, len);
    dest[len] = 0;

    return res;
}

/**
 * strnchr - Find a character in a length limited string
 * \param    s: The string to be searched
 * \param    count: The number of characters to be searched
 * \param    c: The character to search for
 */
char *strnchr(const char *s, size_t count, int c)
{
    for(; count-- && *s != '\0'; ++s)
    {
        if(*s == c)
        {
            return (char *)s;
        }
    }

    return NULL;
}

/**
 * skip_spaces - Removes leading whitespace from str.
 * \param    str: The string to be stripped.
 *
 * Returns a pointer to the first non-whitespace character in str.
 */
char *skip_spaces(const char *str)
{
    while(isspace(*str))
    {
        ++str;
    }

    return (char *)str;
}

/**
 * strim - Removes leading and trailing whitespace from s.
 * \param    s: The string to be stripped.
 *
 * Note that the first trailing whitespace is replaced with a %NUL-terminator
 * in the given string s. Returns a pointer to the first non-whitespace
 * character in s.
 */
char *strim(char *s)
{
    size_t size;
    char  *end;

    s = skip_spaces(s);
    size = strlen(s);
    if(!size)
    {
        return s;
    }

    end = s + size - 1;
    while((end >= s) && isspace(*end))
    {
        end--;
    }
    *(end + 1) = '\0';

    return s;
}

/**
 * strsep - Split a string into tokens
 * \param    s: The string to be searched
 * \param    ct: The characters to search for
 *
 * strsep() updates s to point after the token, ready for the next call.
 *
 * It returns empty tokens, too, behaving exactly like the libc function
 * of that name. In fact, it was stolen from glibc2 and de-fancy-fied.
 * Same semantics, slimmer shape. ;)
 */
char *strsep(char **s, const char *ct)
{
    char *sbegin = *s;
    char *end;

    if(sbegin == NULL)
    {
        return NULL;
    }

    end = strpbrk(sbegin, ct);
    if(end)
    {
        *end++ = '\0';
    }

    *s = end;

    return sbegin;
}

/**
 * memscan - Find a character in an area of memory.
 * \param    addr: The memory area
 * \param    c: The byte to search for
 * \param    size: The size of the area.
 *
 * returns the address of the first occurrence of c, or 1 byte past
 * the area if c is not found
 */
void *memscan(void *addr, int c, size_t size)
{
    unsigned char *p = addr;

    while(size)
    {
        if(*p == c)
        {
            return (void *)p;
        }

        p++;
        size--;
    }

    return (void *)p;
}

/**
 * strnstr - Find the first substring in a length-limited string
 * \param    s1: The string to be searched
 * \param    s2: The string to search for
 * \param    len: the maximum number of characters to search
 */
char *strnstr(const char *s1, const char *s2, size_t len)
{
    size_t l2;

    l2 = strlen(s2);
    if(!l2)
    {
        return (char *)s1;
    }

    while(len >= l2)
    {
        len--;
        if(!memcmp(s1, s2, l2))
        {
            return (char *)s1;
        }
        s1++;
    }

    return NULL;
}

/**
 * strnlen - Find the length of a length-limited string
 * \param    s: The string to be sized
 * \param    count: The maximum number of bytes to search
 */
size_t strnlen(const char *s, size_t count)
{
    const char *sc;

    for(sc = s; count-- && *sc != '\0'; ++sc)
        /* nothing */;

    return sc - s;
}

int atoi(const char *s)
{
	u32 n,i,sign;

	for(i=0;isspace(s[i]);i++);

	sign = (s[i]=='-')? -1:1;

	if (s[i] =='+'||s[i]=='-') {
		i++;
	}
	for (n=0;isdigit(s[i]);i++) {

		n = n*10 + s[i]-'0';
	}
	n = sign * n;

	return n;
}

static int str2hex(char *s)
{
	int val = 0;
	char c;

	if (strncmp(s, "0x", 2) && strncmp(s, "0X", 2)) {
		return 0;
	}

	/* skip 0x */
	s += 2;

	if (strlen(s) > 8) {
		return 0;
	}

	while (*s != 0) {
		c = *s++;

		/* 0 - 9 */
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A' && c <= 'F')
			c = c - 'A' + 10;
		else if (c >= 'a' && c <= 'f')
			c = c - 'a' + 10;
		else {
			break;
		}

		val = val * 16 + c;
	}

	return val;
}

int str2num(char *s)
{
	if (strncmp(s, "0x", 2) == 0 || strncmp(s, "0X", 2) == 0)
		return str2hex(s);
	else
		return atoi(s);
}
