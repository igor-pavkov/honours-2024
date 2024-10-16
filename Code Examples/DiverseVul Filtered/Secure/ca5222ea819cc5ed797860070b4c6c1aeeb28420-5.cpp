static int kex_string_pair(unsigned char **sp,   /* parsing position */
                           unsigned char *data,  /* start pointer to packet */
                           size_t data_len,      /* size of total packet */
                           size_t *lenp,         /* length of the string */
                           unsigned char **strp) /* pointer to string start */
{
    unsigned char *s = *sp;
    *lenp = _libssh2_ntohu32(s);

    /* the length of the string must fit within the current pointer and the
       end of the packet */
    if (*lenp > (data_len - (s - data) -4))
        return 1;
    *strp = s + 4;
    s += 4 + *lenp;

    *sp = s;
    return 0;
}