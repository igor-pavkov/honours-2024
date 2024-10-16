char *str2iso(dst, src)
    char *dst;                          /* destination buffer */
    register ZCONST char *src;          /* source string */
{
#ifdef INTERN_TO_ISO
    INTERN_TO_ISO(src, dst);
#else
    register uch c;
    register char *dstp = dst;

    do {
        c = (uch)foreign(*src++);
        *dstp++ = (char)ASCII2ISO(c);
    } while (c != '\0');
#endif

    return dst;
}