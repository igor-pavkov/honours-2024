static const char *wsgi_callable_object(request_rec *r, const char *s)
{
    const char *name = NULL;
    const char *value = NULL;

    if (!s)
        return "application";

    if (*s != '%')
        return s;

    name = s + 1;

    if (!*name)
        return "application";

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

            if (value)
                return value;
        }
    }

    return "application";
}