static void *wsgi_merge_dir_config(apr_pool_t *p, void *base_conf,
                                   void *new_conf)
{
    WSGIDirectoryConfig *config = NULL;
    WSGIDirectoryConfig *parent = NULL;
    WSGIDirectoryConfig *child = NULL;

    config = newWSGIDirectoryConfig(p);

    parent = (WSGIDirectoryConfig *)base_conf;
    child = (WSGIDirectoryConfig *)new_conf;

    if (child->restrict_process)
        config->restrict_process = child->restrict_process;
    else
        config->restrict_process = parent->restrict_process;

    if (child->process_group)
        config->process_group = child->process_group;
    else
        config->process_group = parent->process_group;

    if (child->application_group)
        config->application_group = child->application_group;
    else
        config->application_group = parent->application_group;

    if (child->callable_object)
        config->callable_object = child->callable_object;
    else
        config->callable_object = parent->callable_object;

    if (child->dispatch_script)
        config->dispatch_script = child->dispatch_script;
    else
        config->dispatch_script = parent->dispatch_script;

    if (child->pass_apache_request != -1)
        config->pass_apache_request = child->pass_apache_request;
    else
        config->pass_apache_request = parent->pass_apache_request;

    if (child->pass_authorization != -1)
        config->pass_authorization = child->pass_authorization;
    else
        config->pass_authorization = parent->pass_authorization;

    if (child->script_reloading != -1)
        config->script_reloading = child->script_reloading;
    else
        config->script_reloading = parent->script_reloading;

    if (child->error_override != -1)
        config->error_override = child->error_override;
    else
        config->error_override = parent->error_override;

    if (child->chunked_request != -1)
        config->chunked_request = child->chunked_request;
    else
        config->chunked_request = parent->chunked_request;

    if (child->enable_sendfile != -1)
        config->enable_sendfile = child->enable_sendfile;
    else
        config->enable_sendfile = parent->enable_sendfile;

    if (child->access_script)
        config->access_script = child->access_script;
    else
        config->access_script = parent->access_script;

    if (child->auth_user_script)
        config->auth_user_script = child->auth_user_script;
    else
        config->auth_user_script = parent->auth_user_script;

    if (child->auth_group_script)
        config->auth_group_script = child->auth_group_script;
    else
        config->auth_group_script = parent->auth_group_script;

    if (child->user_authoritative != -1)
        config->user_authoritative = child->user_authoritative;
    else
        config->user_authoritative = parent->user_authoritative;

    if (child->group_authoritative != -1)
        config->group_authoritative = child->group_authoritative;
    else
        config->group_authoritative = parent->group_authoritative;

#if AP_SERVER_MAJORVERSION_NUMBER >= 2
    if (!child->handler_scripts)
        config->handler_scripts = parent->handler_scripts;
    else if (!parent->handler_scripts)
        config->handler_scripts = child->handler_scripts;
    else {
        config->handler_scripts = apr_hash_overlay(p, child->handler_scripts,
                                                   parent->handler_scripts);
    }
#endif

    return config;
}