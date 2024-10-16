static void wsgi_build_environment(request_rec *r)
{
    WSGIRequestConfig *config = NULL;

    const char *value = NULL;
    const char *script_name = NULL;
    const char *path_info = NULL;

    conn_rec *c = r->connection;

    /* Grab request configuration. */

    config = (WSGIRequestConfig *)ap_get_module_config(r->request_config,
                                                       &wsgi_module);

    /* Populate environment with standard CGI variables. */

    ap_add_cgi_vars(r);
    ap_add_common_vars(r);

    /*
     * Mutate a HEAD request into a GET request. This is
     * required because WSGI specification doesn't lay out
     * clearly how WSGI applications should treat a HEAD
     * request. Generally authors of WSGI applications or
     * frameworks take it that they do not need to return any
     * content, but this screws up any Apache output filters
     * which need to see all the response content in order to
     * correctly set up response headers for a HEAD request such
     * that they are the same as a GET request. Thus change a
     * HEAD request into a GET request to ensure that request
     * content is generated. If using Apache 2.X we can skip
     * doing this if we know there is no output filter that
     * might change the content and/or headers.
     */

#if AP_SERVER_MAJORVERSION_NUMBER >= 2
    if (r->method_number == M_GET && r->header_only &&
        r->output_filters->frec->ftype < AP_FTYPE_PROTOCOL)
        apr_table_setn(r->subprocess_env, "REQUEST_METHOD", "GET");
#else
    if (r->method_number == M_GET && r->header_only)
        apr_table_setn(r->subprocess_env, "REQUEST_METHOD", "GET");
#endif

    /* Determine whether connection uses HTTPS protocol. */

#if AP_SERVER_MAJORVERSION_NUMBER >= 2
    if (!wsgi_is_https)
        wsgi_is_https = APR_RETRIEVE_OPTIONAL_FN(ssl_is_https);

    if (wsgi_is_https && wsgi_is_https(r->connection))
        apr_table_set(r->subprocess_env, "HTTPS", "1");
#endif

    /*
     * If enabled, pass along authorisation headers which Apache
     * leaves out of CGI environment. WSGI still needs to see
     * these if it needs to implement any of the standard
     * authentication schemes such as Basic and Digest. We do
     * not pass these through by default though as it can result
     * in passwords being leaked though to a WSGI application
     * when it shouldn't. This would be a problem where there is
     * some sort of site wide authorisation scheme in place
     * which has got nothing to do with specific applications.
     */

    if (config->pass_authorization) {
        value = apr_table_get(r->headers_in, "Authorization");
        if (value)
            apr_table_setn(r->subprocess_env, "HTTP_AUTHORIZATION", value);
    }

    /* If PATH_INFO not set, set it to an empty string. */

    value = apr_table_get(r->subprocess_env, "PATH_INFO");
    if (!value)
        apr_table_setn(r->subprocess_env, "PATH_INFO", "");

    /*
     * Multiple slashes are not always collapsed into a single
     * slash in SCRIPT_NAME and PATH_INFO with Apache 1.3 and
     * Apache 2.X behaving a bit differently. Because some WSGI
     * applications don't deal with multiple slashes properly we
     * collapse any duplicate slashes to a single slash so
     * Apache behaviour is consistent across all versions. We
     * don't care that PATH_TRANSLATED can on Apache 1.3 still
     * contain multiple slashes as that should not be getting
     * used from a WSGI application anyway.
     */

    script_name = apr_table_get(r->subprocess_env, "SCRIPT_NAME");

    if (*script_name) {
        while (*script_name && (*(script_name+1) == '/'))
            script_name++;
        script_name = apr_pstrdup(r->pool, script_name);
        ap_no2slash((char*)script_name);
        apr_table_setn(r->subprocess_env, "SCRIPT_NAME", script_name);
    }

    path_info = apr_table_get(r->subprocess_env, "PATH_INFO");

    if (*path_info) {
        while (*path_info && (*(path_info+1) == '/'))
            path_info++;
        path_info = apr_pstrdup(r->pool, path_info);
        ap_no2slash((char*)path_info);
        apr_table_setn(r->subprocess_env, "PATH_INFO", path_info);
    }

    /*
     * Set values specific to mod_wsgi configuration. These control
     * aspects of how a request is managed but don't strictly need
     * to be passed through to the application itself. It is though
     * easier to set them here as then they are carried across to
     * the daemon process as part of the environment where they can
     * be extracted and used.
     */

    apr_table_setn(r->subprocess_env, "mod_wsgi.process_group",
                   config->process_group);
    apr_table_setn(r->subprocess_env, "mod_wsgi.application_group",
                   config->application_group);
    apr_table_setn(r->subprocess_env, "mod_wsgi.callable_object",
                   config->callable_object);

    apr_table_setn(r->subprocess_env, "mod_wsgi.request_handler", r->handler);
    apr_table_setn(r->subprocess_env, "mod_wsgi.handler_script",
                   config->handler_script);

    apr_table_setn(r->subprocess_env, "mod_wsgi.script_reloading",
                   apr_psprintf(r->pool, "%d", config->script_reloading));

#if defined(MOD_WSGI_WITH_DAEMONS)
    apr_table_setn(r->subprocess_env, "mod_wsgi.listener_host",
                   c->local_addr->hostname ? c->local_addr->hostname : "");
    apr_table_setn(r->subprocess_env, "mod_wsgi.listener_port",
                   apr_psprintf(r->pool, "%d", c->local_addr->port));
#endif

    apr_table_setn(r->subprocess_env, "mod_wsgi.input_chunked",
                   apr_psprintf(r->pool, "%d", !!r->read_chunked));

    apr_table_setn(r->subprocess_env, "mod_wsgi.enable_sendfile",
                   apr_psprintf(r->pool, "%d", config->enable_sendfile));

#if defined(MOD_WSGI_WITH_DAEMONS)
    apr_table_setn(r->subprocess_env, "mod_wsgi.queue_start",
                   apr_psprintf(r->pool, "%" APR_TIME_T_FMT, r->request_time));
#endif
}