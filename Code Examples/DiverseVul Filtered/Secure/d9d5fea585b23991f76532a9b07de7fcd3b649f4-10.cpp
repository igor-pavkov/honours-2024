static const char *wsgi_server_group(request_rec *r, const char *s)
{
    const char *name = NULL;
    const char *value = NULL;

    const char *h = NULL;
    apr_port_t p = 0;

    if (!s)
        return "";

    if (*s != '%')
        return s;

    name = s + 1;

    if (*name) {
        if (!strcmp(name, "{SERVER}")) {
            h = r->server->server_hostname;
            p = ap_get_server_port(r);

            if (p != DEFAULT_HTTP_PORT && p != DEFAULT_HTTPS_PORT)
                return apr_psprintf(r->pool, "%s:%u", h, p);
            else
                return h;
        }

        if (!strcmp(name, "{GLOBAL}"))
            return "";
    }

    return s;
}