int ssl_cipher_id_cmp(const SSL_CIPHER *a, const SSL_CIPHER *b)
{
    long l;

    l = a->id - b->id;
    if (l == 0L)
        return (0);
    else
        return ((l > 0) ? 1 : -1);
}