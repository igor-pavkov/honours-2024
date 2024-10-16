int SSL_set_ssl_method(SSL *s, const SSL_METHOD *meth)
{
    int conn = -1;
    int ret = 1;

    if (s->method != meth) {
        if (s->handshake_func != NULL)
            conn = (s->handshake_func == s->method->ssl_connect);

        if (s->method->version == meth->version)
            s->method = meth;
        else {
            s->method->ssl_free(s);
            s->method = meth;
            ret = s->method->ssl_new(s);
        }

        if (conn == 1)
            s->handshake_func = meth->ssl_connect;
        else if (conn == 0)
            s->handshake_func = meth->ssl_accept;
    }
    return (ret);
}