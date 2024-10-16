static void wsgi_python_version(void)
{
    const char *compile = PY_VERSION;
    const char *dynamic = 0;

    dynamic = strtok((char *)Py_GetVersion(), " ");

    if (strcmp(compile, dynamic) != 0) {
        ap_log_error(APLOG_MARK, WSGI_LOG_WARNING(0), wsgi_server,
                     "mod_wsgi: Compiled for Python/%s.", compile);
        ap_log_error(APLOG_MARK, WSGI_LOG_WARNING(0), wsgi_server,
                     "mod_wsgi: Runtime using Python/%s.", dynamic);
    }
}