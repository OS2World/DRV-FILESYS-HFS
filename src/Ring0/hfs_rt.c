/*
 * HFS/2 - A Hierarchical File System Driver for OS/2
 * GPL v2 or later.  Written 2026 for the Open Watcom port.
 *
 * hfs_rt.c - the few C library routines the Ring 0 code needs.  An IFS runs
 * without the C run-time start-up, so the library members that drag in the
 * start code (and the stack checker) must not be linked.  Compile with -s -zu.
 */

typedef unsigned int size_t;

void far *memcpy(void far *dst, const void far *src, size_t n)
{
    unsigned char far *d = (unsigned char far *)dst;
    const unsigned char far *s = (const unsigned char far *)src;
    while (n--) *d++ = *s++;
    return dst;
}

char far *strncpy(char far *dst, const char far *src, size_t n)
{
    char far *d = dst;
    while (n && *src) { *d++ = *src++; n--; }
    while (n--) *d++ = 0;
    return dst;
}

size_t strlen(const char far *s)
{
    const char far *p = s;
    while (*p) p++;
    return (size_t)(p - s);
}

char far *strcpy(char far *dst, const char far *src)
{
    char far *d = dst;
    while ((*d++ = *src++) != 0) ;
    return dst;
}

int toupper(int c)
{
    return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}
