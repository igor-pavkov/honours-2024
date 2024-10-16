const COMP_METHOD *SSL_get_current_compression(SSL *s)
{
    if (s->compress != NULL)
        return (s->compress->meth);
    return (NULL);
}