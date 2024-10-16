zvoid *memset(buf, init, len)
    register zvoid *buf;        /* buffer location */
    register int init;          /* initializer character */
    register unsigned int len;  /* length of the buffer */
{
    zvoid *start;

    start = buf;
    while (len--)
        *((char *)buf++) = (char)init;
    return start;
}