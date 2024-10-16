int SSL_get_rfd(const SSL *s)
{
    int ret = -1;
    BIO *b, *r;

    b = SSL_get_rbio(s);
    r = BIO_find_type(b, BIO_TYPE_DESCRIPTOR);
    if (r != NULL)
        BIO_get_fd(r, &ret);
    return (ret);
}