static const char *wsgi_application_group(request_rec *r, const char *s)
{
    const char *name = NULL;
    const char *value = NULL;

    const char *h = NULL;
    apr_port_t p = 0;
    const char *n = NULL;

    if (!s) {
        h = r->server->server_hostname;
        p = ap_get_server_port(r);
        n = wsgi_script_name(r);

        if (p != DEFAULT_HTTP_PORT && p != DEFAULT_HTTPS_PORT)
            return apr_psprintf(r->pool, "%s:%u|%s", h, p, n);
        else
            return apr_psprintf(r->pool, "%s|%s", h, n);
    }

    if (*s != '%')
        return s;

    name = s + 1;

    if (*name) {
        if (!strcmp(name, "{RESOURCE}")) {
            h = r->server->server_hostname;
            p = ap_get_server_port(r);
            n = wsgi_script_name(r);

            if (p != DEFAULT_HTTP_PORT && p != DEFAULT_HTTPS_PORT)
                return apr_psprintf(r->pool, "%s:%u|%s", h, p, n);
            else
                return apr_psprintf(r->pool, "%s|%s", h, n);
        }

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

        if (strstr(name, "{ENV:") == name) {
            int len = 0;

            name = name + 5;
            len = strlen(name);

            if (len && name[len-1] == '}') {
                name = apr_pstrndup(r->pool, name, len-1);

                value = apr_table_get(r->notes, name);

                if (!value)
                    value = apr_table_get(r->subprocess_env, name);

                if (!value)
                    value = getenv(name);

                if (value) {
                    if (*value == '%' && strstr(value, "%{ENV:") != value)
                        return wsgi_application_group(r, value);

                    return value;
                }
            }
        }
    }

    return s;
}