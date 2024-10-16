int ssl_cipher_ptr_id_cmp(const SSL_CIPHER *const *ap,
                          const SSL_CIPHER *const *bp)
{
    long l;

    l = (*ap)->id - (*bp)->id;
    if (l == 0L)
        return (0);
    else
        return ((l > 0) ? 1 : -1);
}