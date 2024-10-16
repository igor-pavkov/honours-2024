const char *SSL_get_servername(const SSL *s, const int type)
{
    if (type != TLSEXT_NAMETYPE_host_name)
        return NULL;

    return s->session && !s->tlsext_hostname ?
        s->session->tlsext_hostname : s->tlsext_hostname;
}