zvoid *memcpy(dst, src, len)
    register zvoid *dst;
    register ZCONST zvoid *src;
    register unsigned int len;
{
    zvoid *start;

    start = dst;
    while (len-- > 0)
        *((char *)dst)++ = *((ZCONST char *)src)++;
    return start;
}