const char *SSL_get_version(const SSL *s)
{
    if (s->version == TLS1_2_VERSION)
        return ("TLSv1.2");
    else if (s->version == TLS1_1_VERSION)
        return ("TLSv1.1");
    else if (s->version == TLS1_VERSION)
        return ("TLSv1");
    else if (s->version == SSL3_VERSION)
        return ("SSLv3");
    else if (s->version == SSL2_VERSION)
        return ("SSLv2");
    else
        return ("unknown");
}