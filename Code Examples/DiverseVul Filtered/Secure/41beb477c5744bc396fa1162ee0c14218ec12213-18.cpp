char *str2oem(dst, src)
    char *dst;                          /* destination buffer */
    register ZCONST char *src;          /* source string */
{
#ifdef INTERN_TO_OEM
    INTERN_TO_OEM(src, dst);
#else
    register uch c;
    register char *dstp = dst;

    do {
        c = (uch)foreign(*src++);
        *dstp++ = (char)ASCII2OEM(c);
    } while (c != '\0');
#endif

    return dst;
}