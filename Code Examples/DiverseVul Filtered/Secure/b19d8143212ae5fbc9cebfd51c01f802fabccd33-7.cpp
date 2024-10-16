static const SSL_METHOD *ssl3_get_server_method(int ver)
{
    if (ver == SSL3_VERSION)
        return (SSLv3_server_method());
    else
        return (NULL);
}