static const SSL_METHOD *ssl3_get_client_method(int ver)
{
    if (ver == SSL3_VERSION)
        return (SSLv3_client_method());
    else
        return (NULL);
}