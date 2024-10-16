static const char *wsgi_set_python_hash_seed(cmd_parms *cmd, void *mconfig,
                                             const char *f)
{
    const char *error = NULL;
    WSGIServerConfig *sconfig = NULL;

    error = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (error != NULL)
        return error;

    /*
     * Must check this here because if we don't and is wrong, then
     * Python interpreter will check later and may kill the process.
     */

    if (f && *f != '\0' && strcmp(f, "random") != 0) {
        const char *endptr = f;
        unsigned long seed;

        seed = PyOS_strtoul((char *)f, (char **)&endptr, 10);

        if (*endptr != '\0' || seed > 4294967295UL
                || (errno == ERANGE && seed == ULONG_MAX))
        {
            return "WSGIPythonHashSeed must be \"random\" or an integer "
                              "in range [0; 4294967295]";
        }
    }

    sconfig = ap_get_module_config(cmd->server->module_config, &wsgi_module);
    sconfig->python_hash_seed = f;

    return NULL;
}