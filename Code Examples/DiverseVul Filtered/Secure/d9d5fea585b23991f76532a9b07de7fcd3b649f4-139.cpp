static void wsgi_hook_init(server_rec *s, apr_pool_t *p)
{
    char package[128];

    /* Setup module version information. */

    sprintf(package, "mod_wsgi/%s", MOD_WSGI_VERSION_STRING);

    ap_add_version_component(package);

    /* Record Python version string with Apache. */

    if (!Py_IsInitialized()) {
        char buffer[256];
        const char *token = NULL;
        const char *version = NULL;
        
        version = Py_GetVersion();

        token = version;
        while (*token && *token != ' ')
            token++;

        strcpy(buffer, "Python/");
        strncat(buffer, version, token - version);

        ap_add_version_component(buffer);
    }

    /* Retain reference to base server. */

    wsgi_server = s;

    /* Retain record of parent process ID. */

    wsgi_parent_pid = getpid();

    /* Determine whether multiprocess and/or multithreaded. */

    wsgi_multiprocess = 1;
    wsgi_multithread = 0;

    /* Retain reference to main server config. */

    wsgi_server_config = ap_get_module_config(s->module_config, &wsgi_module);

    /*
     * Check that the version of Python found at
     * runtime is what was used at compilation.
     */

    wsgi_python_version();

    /*
     * Initialise Python if required to be done in
     * the parent process. Note that it will not be
     * initialised if mod_python loaded and it has
     * already been done.
     */

    if (!wsgi_python_after_fork)
        wsgi_python_init(p);
}