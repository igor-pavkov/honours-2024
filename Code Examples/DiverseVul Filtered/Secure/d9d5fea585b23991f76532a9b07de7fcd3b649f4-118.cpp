static WSGIRequestConfig *wsgi_create_req_config(apr_pool_t *p, request_rec *r)
{
    WSGIRequestConfig *config = NULL;
    WSGIServerConfig *sconfig = NULL;
    WSGIDirectoryConfig *dconfig = NULL;

    config = (WSGIRequestConfig *)apr_pcalloc(p, sizeof(WSGIRequestConfig));

    dconfig = ap_get_module_config(r->per_dir_config, &wsgi_module);
    sconfig = ap_get_module_config(r->server->module_config, &wsgi_module);

    config->pool = p;

    config->restrict_process = dconfig->restrict_process;

    if (!config->restrict_process)
        config->restrict_process = sconfig->restrict_process;

    config->process_group = dconfig->process_group;

    if (!config->process_group)
        config->process_group = sconfig->process_group;

    config->process_group = wsgi_process_group(r, config->process_group);

    config->application_group = dconfig->application_group;

    if (!config->application_group)
        config->application_group = sconfig->application_group;

    config->application_group = wsgi_application_group(r,
                                    config->application_group);

    config->callable_object = dconfig->callable_object;

    if (!config->callable_object)
        config->callable_object = sconfig->callable_object;

    config->callable_object = wsgi_callable_object(r, config->callable_object);

    config->dispatch_script = dconfig->dispatch_script;

    if (!config->dispatch_script)
        config->dispatch_script = sconfig->dispatch_script;

    config->pass_apache_request = dconfig->pass_apache_request;

    if (config->pass_apache_request < 0) {
        config->pass_apache_request = sconfig->pass_apache_request;
        if (config->pass_apache_request < 0)
            config->pass_apache_request = 0;
    }

    config->pass_authorization = dconfig->pass_authorization;

    if (config->pass_authorization < 0) {
        config->pass_authorization = sconfig->pass_authorization;
        if (config->pass_authorization < 0)
            config->pass_authorization = 0;
    }

    config->script_reloading = dconfig->script_reloading;

    if (config->script_reloading < 0) {
        config->script_reloading = sconfig->script_reloading;
        if (config->script_reloading < 0)
            config->script_reloading = 1;
    }

    config->error_override = dconfig->error_override;

    if (config->error_override < 0) {
        config->error_override = sconfig->error_override;
        if (config->error_override < 0)
            config->error_override = 0;
    }

    config->chunked_request = dconfig->chunked_request;

    if (config->chunked_request < 0) {
        config->chunked_request = sconfig->chunked_request;
        if (config->chunked_request < 0)
            config->chunked_request = 0;
    }

    config->enable_sendfile = dconfig->enable_sendfile;

    if (config->enable_sendfile < 0) {
        config->enable_sendfile = sconfig->enable_sendfile;
        if (config->enable_sendfile < 0)
            config->enable_sendfile = 0;
    }

    config->access_script = dconfig->access_script;

    config->auth_user_script = dconfig->auth_user_script;

    config->auth_group_script = dconfig->auth_group_script;

    config->user_authoritative = dconfig->user_authoritative;

    if (config->user_authoritative == -1)
        config->user_authoritative = 1;

    config->group_authoritative = dconfig->group_authoritative;

    if (config->group_authoritative == -1)
        config->group_authoritative = 1;

#if AP_SERVER_MAJORVERSION_NUMBER >= 2
    if (!dconfig->handler_scripts)
        config->handler_scripts = sconfig->handler_scripts;
    else if (!sconfig->handler_scripts)
        config->handler_scripts = dconfig->handler_scripts;
    else {
        config->handler_scripts = apr_hash_overlay(p, dconfig->handler_scripts,
                                                   sconfig->handler_scripts);
    }
#endif

    config->handler_script = "";

    return config;
}