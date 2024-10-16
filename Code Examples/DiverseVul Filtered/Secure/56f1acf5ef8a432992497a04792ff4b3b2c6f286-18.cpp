int SSL_get_verify_mode(const SSL *s)
{
    return (s->verify_mode);
}