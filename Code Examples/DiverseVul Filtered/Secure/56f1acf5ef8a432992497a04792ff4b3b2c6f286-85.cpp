int SSL_want(const SSL *s)
{
    return (s->rwstate);
}