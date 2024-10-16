EVP_PKEY *SSL_get_privatekey(SSL *s)
{
    if (s->cert != NULL)
        return (s->cert->key->privatekey);
    else
        return (NULL);
}