static int req_parsebody(lua_State *L)
{
    apr_array_header_t          *pairs;
    apr_off_t len;
    int res;
    apr_size_t size;
    apr_size_t max_post_size;
    char *multipart;
    const char *contentType;
    request_rec *r = ap_lua_check_request_rec(L, 1);
    max_post_size = (apr_size_t) luaL_optint(L, 2, MAX_STRING_LEN);
    multipart = apr_pcalloc(r->pool, 256);
    contentType = apr_table_get(r->headers_in, "Content-Type");
    lua_newtable(L);
    lua_newtable(L);            /* [table, table] */    
    if (contentType != NULL && (sscanf(contentType, "multipart/form-data; boundary=%250c", multipart) == 1)) {
        char        *buffer, *key, *filename;
        char        *start = 0, *end = 0, *crlf = 0;
        const char  *data;
        int         i;
        size_t      vlen = 0;
        size_t      len = 0;
        if (lua_read_body(r, &data, (apr_off_t*) &size, max_post_size) != OK) {
            return 2;
        }
        len = strlen(multipart);
        i = 0;
        for
        (
            start = strstr((char *) data, multipart);
            start != NULL;
            start = end
        ) {
            i++;
            if (i == POST_MAX_VARS) break;
            crlf = strstr((char *) start, "\r\n\r\n");
            if (!crlf) break;
            end = ap_lua_binstrstr(crlf, (size - (crlf - data)), multipart, len);
            if (end == NULL) break;
            key = (char *) apr_pcalloc(r->pool, 256);
            filename = (char *) apr_pcalloc(r->pool, 256);
            vlen = end - crlf - 8;
            buffer = (char *) apr_pcalloc(r->pool, vlen+1);
            memcpy(buffer, crlf + 4, vlen);
            sscanf(start + len + 2,
                "Content-Disposition: form-data; name=\"%255[^\"]\"; filename=\"%255[^\"]\"",
                key, filename);
            if (strlen(key)) {
                req_aprtable2luatable_cb_len(L, key, buffer, vlen);
            }
        }
    }
    else {
        char *buffer;
        res = ap_parse_form_data(r, NULL, &pairs, -1, max_post_size);
        if (res == OK) {
            while(pairs && !apr_is_empty_array(pairs)) {
                ap_form_pair_t *pair = (ap_form_pair_t *) apr_array_pop(pairs);
                apr_brigade_length(pair->value, 1, &len);
                size = (apr_size_t) len;
                buffer = apr_palloc(r->pool, size + 1);
                apr_brigade_flatten(pair->value, buffer, &size);
                buffer[len] = 0;
                req_aprtable2luatable_cb(L, pair->name, buffer);
            }
        }
    }
    return 2;                   /* [table<string, string>, table<string, array<string>>] */
}