static const char *wsgi_set_accept_mutex(cmd_parms *cmd, void *mconfig,
                                         const char *arg)
{
    const char *error = NULL;
    WSGIServerConfig *sconfig = NULL;

    error = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (error != NULL)
        return error;

    sconfig = ap_get_module_config(cmd->server->module_config, &wsgi_module);

#if !defined(AP_ACCEPT_MUTEX_TYPE)
    sconfig->lock_mechanism = ap_accept_lock_mech;
#else
    sconfig->lock_mechanism = APR_LOCK_DEFAULT;
#endif

    if (!strcasecmp(arg, "default")) {
        sconfig->lock_mechanism = APR_LOCK_DEFAULT;
    }
#if APR_HAS_FLOCK_SERIALIZE
    else if (!strcasecmp(arg, "flock")) {
        sconfig->lock_mechanism = APR_LOCK_FLOCK;
    }
#endif
#if APR_HAS_FCNTL_SERIALIZE
    else if (!strcasecmp(arg, "fcntl")) {
        sconfig->lock_mechanism = APR_LOCK_FCNTL;
    }
#endif
#if APR_HAS_SYSVSEM_SERIALIZE && !defined(PERCHILD_MPM)
    else if (!strcasecmp(arg, "sysvsem")) {
        sconfig->lock_mechanism = APR_LOCK_SYSVSEM;
    }
#endif
#if APR_HAS_POSIXSEM_SERIALIZE
    else if (!strcasecmp(arg, "posixsem")) {
        sconfig->lock_mechanism = APR_LOCK_POSIXSEM;
    }
#endif
#if APR_HAS_PROC_PTHREAD_SERIALIZE
    else if (!strcasecmp(arg, "pthread")) {
        sconfig->lock_mechanism = APR_LOCK_PROC_PTHREAD;
    }
#endif
    else {
        return apr_pstrcat(cmd->pool, "Accept mutex lock mechanism '", arg,
                           "' is invalid. ", wsgi_valid_accept_mutex_string,
                           NULL);
    }

    return NULL;
}