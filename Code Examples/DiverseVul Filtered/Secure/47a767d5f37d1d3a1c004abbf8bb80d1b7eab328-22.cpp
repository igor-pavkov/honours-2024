char *am_extract_query_parameter(apr_pool_t *pool,
                                 const char *query_string,
                                 const char *name)
{
    const char *ip;
    const char *value_end;
    apr_size_t namelen;

    if (query_string == NULL) {
        return NULL;
    }

    ip = query_string;
    namelen = strlen(name);

    /* Find parameter. Searches for /[^&]<name>[&=$]/.
     * Moves ip to the first character after the name (either '&', '='
     * or '\0').
     */
    for (;;) {
        /* First we find the name of the parameter. */
        ip = strstr(ip, name);
        if (ip == NULL) {
            /* Parameter not found. */
            return NULL;
        }

        /* Then we check what is before the parameter name. */
        if (ip != query_string && ip[-1] != '&') {
            /* Name not preceded by [^&]. */
            ip++;
            continue;
        }

        /* And last we check what follows the parameter name. */
        if (ip[namelen] != '=' && ip[namelen] != '&'
            && ip[namelen] != '\0') {
            /* Name not followed by [&=$]. */
            ip++;
            continue;
        }


        /* We have found the pattern. */
        ip += namelen;
        break;
    }

    /* Now ip points to the first character after the name. If this
     * character is '&' or '\0', then this field doesn't have a value.
     * If this character is '=', then this field has a value.
     */
    if (ip[0] == '=') {
        ip += 1;
    }

    /* The value is from ip to '&' or to the end of the string, whichever
     * comes first. */
    value_end = strchr(ip, '&');
    if (value_end != NULL) {
        /* '&' comes first. */
        return apr_pstrndup(pool, ip, value_end - ip);
    } else {
        /* Value continues until the end of the string. */
        return apr_pstrdup(pool, ip);
    }
}