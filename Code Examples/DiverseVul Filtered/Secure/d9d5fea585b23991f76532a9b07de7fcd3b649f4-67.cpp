static int wsgi_hook_handler(request_rec *r)
{
    int status;
    apr_off_t limit = 0;

    WSGIRequestConfig *config = NULL;

    const char *value = NULL;

    /* Filter out the obvious case of no handler defined. */

    if (!r->handler)
        return DECLINED;

    /*
     * Construct request configuration and cache it in the
     * request object against this module so can access it later
     * from handler code.
     */

    config = wsgi_create_req_config(r->pool, r);

    ap_set_module_config(r->request_config, &wsgi_module, config);

    /*
     * Only process requests for this module. First check for
     * where target is the actual WSGI script. Then need to
     * check for the case where handler name mapped to a handler
     * script definition.
     */

    if (!strcmp(r->handler, "wsgi-script") ||
        !strcmp(r->handler, "application/x-httpd-wsgi")) {

        /*
         * Ensure that have adequate privileges to run the WSGI
         * script. Require ExecCGI to be specified in Options for
         * this. In doing this, using the wider interpretation that
         * ExecCGI refers to any executable like script even though
         * not a separate process execution.
         */

        if (!(ap_allow_options(r) & OPT_EXECCGI) &&
            !wsgi_is_script_aliased(r)) {
            wsgi_log_script_error(r, "Options ExecCGI is off in this "
                                  "directory", r->filename);
            return HTTP_FORBIDDEN;
        }

        /* Ensure target script exists and is a file. */

#if AP_SERVER_MAJORVERSION_NUMBER < 2
        if (r->finfo.st_mode == 0) {
            wsgi_log_script_error(r, "Target WSGI script not found or unable "
                                  "to stat", r->filename);
            return HTTP_NOT_FOUND;
        }
#else
        if (r->finfo.filetype == 0) {
            wsgi_log_script_error(r, "Target WSGI script not found or unable "
                                  "to stat", r->filename);
            return HTTP_NOT_FOUND;
        }
#endif

#if AP_SERVER_MAJORVERSION_NUMBER < 2
        if (S_ISDIR(r->finfo.st_mode)) {
            wsgi_log_script_error(r, "Attempt to invoke directory as WSGI "
                                  "application", r->filename);
            return HTTP_FORBIDDEN;
        }
#else
        if (r->finfo.filetype == APR_DIR) {
            wsgi_log_script_error(r, "Attempt to invoke directory as WSGI "
                                  "application", r->filename);
            return HTTP_FORBIDDEN;
        }
#endif

        if (wsgi_is_script_aliased(r)) {
            /*
             * Allow any configuration supplied through request notes to
             * override respective values. Request notes are used when
             * configuration supplied with WSGIScriptAlias directives.
             */

            if (value = apr_table_get(r->notes, "mod_wsgi.process_group"))
                config->process_group = wsgi_process_group(r, value);
            if (value = apr_table_get(r->notes, "mod_wsgi.application_group"))
                config->application_group = wsgi_application_group(r, value);
            if (value = apr_table_get(r->notes, "mod_wsgi.callable_object"))
                config->callable_object = value;

            if (value = apr_table_get(r->notes,
                                      "mod_wsgi.pass_authorization")) {
                if (!strcmp(value, "1"))
                    config->pass_authorization = 1;
                else
                    config->pass_authorization = 0;
            }
        }
    }
#if AP_SERVER_MAJORVERSION_NUMBER >= 2
    else if (config->handler_scripts) {
        WSGIScriptFile *entry;

        entry = (WSGIScriptFile *)apr_hash_get(config->handler_scripts,
                                               r->handler,
                                               APR_HASH_KEY_STRING);

        if (entry) {
            config->handler_script = entry->handler_script;
            config->callable_object = "handle_request";

            if (value = entry->process_group)
                config->process_group = wsgi_process_group(r, value);
            if (value = entry->application_group)
                config->application_group = wsgi_application_group(r, value);

            if (value = entry->pass_authorization) {
                if (!strcmp(value, "1"))
                    config->pass_authorization = 1;
                else
                    config->pass_authorization = 0;
            }
        }
        else
            return DECLINED;
    }
#endif
    else
        return DECLINED;

    /*
     * For Apache 2.0+ honour AcceptPathInfo directive. Default
     * behaviour is accept additional path information. Under
     * Apache 1.3, WSGI application would need to check itself.
     */

#if AP_MODULE_MAGIC_AT_LEAST(20011212,0)
    if ((r->used_path_info == AP_REQ_REJECT_PATH_INFO) &&
        r->path_info && *r->path_info) {
        wsgi_log_script_error(r, "AcceptPathInfo off disallows user's path",
                              r->filename);
        return HTTP_NOT_FOUND;
    }
#endif

    /*
     * Setup policy to apply if request contains a body. Note
     * that WSGI specification doesn't strictly allow for chunked
     * request content as CONTENT_LENGTH required when reading
     * input and application isn't meant to read more than what
     * is defined by CONTENT_LENGTH. To allow chunked request
     * content tell Apache to dechunk it. For application to use
     * the content, it has to ignore WSGI specification and use
     * read() with no arguments to read all available input, or
     * call read() with specific block size until read() returns
     * an empty string.
     */

    if (config->chunked_request)
        status = ap_setup_client_block(r, REQUEST_CHUNKED_DECHUNK);
    else
        status = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR);

    if (status != OK)
        return status;

    /*
     * Check to see if request content is too large and end
     * request here. We do this as otherwise it will not be done
     * until first time input data is read in application.
     * Problem is that underlying HTTP output filter will
     * also generate a 413 response and the error raised from
     * the application will be appended to that. The call to
     * ap_discard_request_body() is hopefully enough to trigger
     * sending of the 413 response by the HTTP filter.
     */

    limit = ap_get_limit_req_body(r);

    if (limit && limit < r->remaining) {
        ap_discard_request_body(r);
        return OK;
    }

    /* Build the sub process environment. */

    wsgi_build_environment(r);

    /*
     * If a dispatch script has been provided, as appropriate
     * allow it to override any of the configuration related
     * to what context the script will be executed in and what
     * the target callable object for the application is.
     */

    if (config->dispatch_script) {
        status = wsgi_execute_dispatch(r);

        if (status != OK)
            return status;
    }

    /*
     * Execute the target WSGI application script or proxy
     * request to one of the daemon processes as appropriate.
     */

#if defined(MOD_WSGI_WITH_DAEMONS)
    status = wsgi_execute_remote(r);

    if (status != DECLINED)
        return status;
#endif

#if defined(MOD_WSGI_DISABLE_EMBEDDED)
    wsgi_log_script_error(r, "Embedded mode of mod_wsgi disabled at compile "
                          "time", r->filename);
    return HTTP_INTERNAL_SERVER_ERROR;
#endif

    if (wsgi_server_config->restrict_embedded == 1) {
        wsgi_log_script_error(r, "Embedded mode of mod_wsgi disabled by "
                              "runtime configuration", r->filename);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return wsgi_execute_script(r);
}