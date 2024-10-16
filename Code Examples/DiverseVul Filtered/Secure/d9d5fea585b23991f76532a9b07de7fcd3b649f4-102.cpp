static apr_status_t wsgi_python_parent_cleanup(void *data)
#endif
{
    if (wsgi_parent_pid == getpid()) {
        /*
         * Destroy Python itself including the main
         * interpreter. If mod_python is being loaded it
         * is left to mod_python to destroy Python,
         * although it currently doesn't do so.
         */

        if (wsgi_python_initialized)
            wsgi_python_term();
    }

#if AP_SERVER_MAJORVERSION_NUMBER >= 2
    return APR_SUCCESS;
#endif
}