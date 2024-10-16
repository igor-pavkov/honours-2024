X509 *SSL_get_certificate(const SSL *s)
{
    if (s->cert != NULL)
        return (s->cert->key->x509);
    else
        return (NULL);
}