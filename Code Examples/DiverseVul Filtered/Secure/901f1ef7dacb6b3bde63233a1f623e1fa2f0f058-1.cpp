int RSA_padding_add_SSLv23(unsigned char *to, int tlen,
                           const unsigned char *from, int flen)
{
    int i, j;
    unsigned char *p;

    if (flen > (tlen - RSA_PKCS1_PADDING_SIZE)) {
        RSAerr(RSA_F_RSA_PADDING_ADD_SSLV23,
               RSA_R_DATA_TOO_LARGE_FOR_KEY_SIZE);
        return 0;
    }

    p = (unsigned char *)to;

    *(p++) = 0;
    *(p++) = 2;                 /* Public Key BT (Block Type) */

    /* pad out with non-zero random data */
    j = tlen - 3 - 8 - flen;

    if (RAND_bytes(p, j) <= 0)
        return 0;
    for (i = 0; i < j; i++) {
        if (*p == '\0')
            do {
                if (RAND_bytes(p, 1) <= 0)
                    return 0;
            } while (*p == '\0');
        p++;
    }

    memset(p, 3, 8);
    p += 8;
    *(p++) = '\0';

    memcpy(p, from, (unsigned int)flen);
    return 1;
}