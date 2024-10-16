int memcmp(b1, b2, len)
    register ZCONST zvoid *b1;
    register ZCONST zvoid *b2;
    register unsigned int len;
{
    register int c;

    if (len > 0) do {
        if ((c = (int)(*((ZCONST unsigned char *)b1)++) -
                 (int)(*((ZCONST unsigned char *)b2)++)) != 0)
           return c;
    } while (--len > 0)
    return 0;
}