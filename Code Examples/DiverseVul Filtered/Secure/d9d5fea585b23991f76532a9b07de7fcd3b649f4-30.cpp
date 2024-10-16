static const char *wsgi_script_name(request_rec *r)
{
    char *script_name = NULL;
    int path_info_start = 0;

    if (!r->path_info || !*r->path_info) {
        script_name = apr_pstrdup(r->pool, r->uri);
    }
    else {
        path_info_start = wsgi_find_path_info(r->uri, r->path_info);

        script_name = apr_pstrndup(r->pool, r->uri, path_info_start);
    }

    if (*script_name) {
        while (*script_name && (*(script_name+1) == '/'))
            script_name++;
        script_name = apr_pstrdup(r->pool, script_name);
        ap_no2slash((char*)script_name);
    }

    ap_str_tolower(script_name);

    return script_name;
}