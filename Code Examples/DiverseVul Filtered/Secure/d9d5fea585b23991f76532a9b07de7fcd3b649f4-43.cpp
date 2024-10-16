static const char *wsgi_set_lazy_initialization(cmd_parms *cmd, void *mconfig,
                                                const char *f)
{
    const char *error = NULL;

    error = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (error != NULL)
        return error;

    if (strcasecmp(f, "Off") == 0)
        wsgi_python_after_fork = 0;
    else if (strcasecmp(f, "On") == 0)
        wsgi_python_after_fork = 1;
    else
        return "WSGILazyInitialization must be one of: Off | On";

    return NULL;
}