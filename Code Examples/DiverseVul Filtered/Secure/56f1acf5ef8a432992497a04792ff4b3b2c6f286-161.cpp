int SSL_get_fd(const SSL *s)
{
    return (SSL_get_rfd(s));
}