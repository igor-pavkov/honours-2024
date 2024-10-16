const COMP_METHOD *SSL_get_current_expansion(SSL *s)
{
    if (s->expand != NULL)
        return (s->expand->meth);
    return (NULL);
}