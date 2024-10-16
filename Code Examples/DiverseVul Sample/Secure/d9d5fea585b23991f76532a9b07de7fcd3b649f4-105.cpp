static WSGIServerConfig *newWSGIServerConfig(apr_pool_t *p)
{
    WSGIServerConfig *object = NULL;

    object = (WSGIServerConfig *)apr_pcalloc(p, sizeof(WSGIServerConfig));

    object->pool = p;

    object->alias_list = NULL;

    object->socket_prefix = NULL;

#if defined(MOD_WSGI_WITH_DAEMONS)
    object->socket_prefix = DEFAULT_REL_RUNTIMEDIR "/wsgi";
    object->socket_prefix = ap_server_root_relative(p, object->socket_prefix);
#endif

    object->verbose_debugging = 0;

    object->python_warnings = NULL;

    object->py3k_warning_flag = -1;
    object->python_optimize = -1;
    object->dont_write_bytecode = -1;

    object->lang = NULL;
    object->locale = NULL;

    object->python_home = NULL;
    object->python_path = NULL;
    object->python_eggs = NULL;

    object->restrict_embedded = -1;
    object->restrict_stdin = -1;
    object->restrict_stdout = -1;
    object->restrict_signal = -1;

#if defined(WIN32) || defined(DARWIN)
    object->case_sensitivity = 0;
#else
    object->case_sensitivity = 1;
#endif

    object->restrict_process = NULL;

    object->process_group = NULL;
    object->application_group = NULL;
    object->callable_object = NULL;

    object->dispatch_script = NULL;

    object->pass_apache_request = -1;
    object->pass_authorization = -1;
    object->script_reloading = -1;
    object->error_override = -1;
    object->chunked_request = -1;

    object->enable_sendfile = -1;

    return object;
}