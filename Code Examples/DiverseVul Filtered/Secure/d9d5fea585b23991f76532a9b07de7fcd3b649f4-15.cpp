static void wsgi_python_init(apr_pool_t *p)
{
    const char *python_home = 0;

#if defined(DARWIN) && (AP_SERVER_MAJORVERSION_NUMBER < 2)
    static int initialized = 0;
#else
    static int initialized = 1;
#endif

    /* Perform initialisation if required. */

    if (!Py_IsInitialized() || !initialized) {

        /* Enable Python 3.0 migration warnings. */

#if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION >= 6
        if (wsgi_server_config->py3k_warning_flag == 1)
            Py_Py3kWarningFlag++;
#endif

        /* Disable writing of byte code files. */

#if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION >= 6
        if (wsgi_server_config->dont_write_bytecode == 1)
            Py_DontWriteBytecodeFlag++;
#endif

        /* Check for Python paths and optimisation flag. */

        if (wsgi_server_config->python_optimize > 0)
            Py_OptimizeFlag = wsgi_server_config->python_optimize;
        else
            Py_OptimizeFlag = 0;

        /* Check for control options for Python warnings. */

        if (wsgi_server_config->python_warnings) {
            apr_array_header_t *options = NULL;
            char **entries;

            int i;

            options = wsgi_server_config->python_warnings;
            entries = (char **)options->elts;

            for (i = 0; i < options->nelts; ++i) {
#if PY_MAJOR_VERSION >= 3
                wchar_t *s = NULL;
                int len = strlen(entries[i])+1;

                s = (wchar_t *)apr_palloc(p, len*sizeof(wchar_t));

#if defined(WIN32) && defined(APR_HAS_UNICODE_FS)
                wsgi_utf8_to_unicode_path(s, len, entries[i]);
#else
                mbstowcs(s, entries[i], len);
#endif
                PySys_AddWarnOption(s);
#else
                PySys_AddWarnOption(entries[i]);
#endif
            }
        }

        /* Check for Python HOME being overridden. */

        python_home = wsgi_server_config->python_home;

#if defined(MOD_WSGI_WITH_DAEMONS)
        if (wsgi_daemon_process && wsgi_daemon_process->group->python_home)
            python_home = wsgi_daemon_process->group->python_home;
#endif

#if PY_MAJOR_VERSION >= 3
        if (python_home) {
            wchar_t *s = NULL;
            int len = strlen(python_home)+1;

            ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                         "mod_wsgi (pid=%d): Python home %s.", getpid(),
                         python_home);

            s = (wchar_t *)apr_palloc(p, len*sizeof(wchar_t));

#if defined(WIN32) && defined(APR_HAS_UNICODE_FS)
            wsgi_utf8_to_unicode_path(s, len, python_home);
#else
            mbstowcs(s, python_home, len);
#endif
            Py_SetPythonHome(s);
        }
#else
        if (python_home) {
            ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                         "mod_wsgi (pid=%d): Python home %s.", getpid(),
                         python_home);

            Py_SetPythonHome((char *)python_home);
        }
#endif

        /*
         * Work around bug in Python 3.1 where it will crash
         * when used in non console application on Windows if
         * stdin/stdout have been initialised and aren't null.
         */

#if defined(WIN32) && PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 3
        _wputenv(L"PYTHONIOENCODING=cp1252:backslashreplace");
#endif

        /* Initialise Python. */

        ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                     "mod_wsgi (pid=%d): Initializing Python.", getpid());

        initialized = 1;

        Py_Initialize();

        /* Initialise threading. */

        PyEval_InitThreads();

        /*
         * We now want to release the GIL. Before we do that
         * though we remember what the current thread state is.
         * We will use that later to restore the main thread
         * state when we want to cleanup interpreters on
         * shutdown.
         */

        wsgi_main_tstate = PyThreadState_Get();
        PyEval_ReleaseThread(wsgi_main_tstate);

        wsgi_python_initialized = 1;

        /*
         * Register cleanups to be performed on parent restart
         * or shutdown. This will destroy Python itself.
         */

#if AP_SERVER_MAJORVERSION_NUMBER < 2
        ap_register_cleanup(p, NULL, wsgi_python_parent_cleanup,
                            ap_null_cleanup);
#else
        apr_pool_cleanup_register(p, NULL, wsgi_python_parent_cleanup,
                                  apr_pool_cleanup_null);
#endif
    }
}