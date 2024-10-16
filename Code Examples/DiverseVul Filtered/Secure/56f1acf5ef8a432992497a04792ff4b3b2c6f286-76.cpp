int SSL_get_servername_type(const SSL *s)
{
    if (s->session
        && (!s->tlsext_hostname ? s->session->
            tlsext_hostname : s->tlsext_hostname))
        return TLSEXT_NAMETYPE_host_name;
    return -1;
}