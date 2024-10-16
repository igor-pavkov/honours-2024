char *plastchar(ptr, len)
    ZCONST char *ptr;
    extent len;
{
    unsigned clen;
    ZCONST char *oldptr = ptr;
    while(*ptr != '\0' && len > 0){
        oldptr = ptr;
        clen = CLEN(ptr);
        ptr += clen;
        len -= clen;
    }
    return (char *)oldptr;
}