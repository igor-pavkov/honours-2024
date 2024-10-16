static PyObject *wsgi_load_source(apr_pool_t *pool, request_rec *r,
                                  const char *name, int exists,
                                  const char* filename,
                                  const char *process_group,
                                  const char *application_group)
{
    FILE *fp = NULL;
    PyObject *m = NULL;
    PyObject *co = NULL;
    struct _node *n = NULL;

#if defined(WIN32) && defined(APR_HAS_UNICODE_FS)
    apr_wchar_t wfilename[APR_PATH_MAX];
#endif

    if (exists) {
        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, WSGI_LOG_INFO(0), r,
                          "mod_wsgi (pid=%d, process='%s', application='%s'): "
                          "Reloading WSGI script '%s'.", getpid(),
                          process_group, application_group, filename);
        }
        else {
            ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                         "mod_wsgi (pid=%d, process='%s', application='%s'): "
                         "Reloading WSGI script '%s'.", getpid(),
                         process_group, application_group, filename);
        }
        Py_END_ALLOW_THREADS
    }
    else {
        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, WSGI_LOG_INFO(0), r,
                          "mod_wsgi (pid=%d, process='%s', application='%s'): "
                          "Loading WSGI script '%s'.", getpid(),
                          process_group, application_group, filename);
        }
        else {
            ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                         "mod_wsgi (pid=%d, process='%s', application='%s'): "
                         "Loading WSGI script '%s'.", getpid(),
                         process_group, application_group, filename);
        }
        Py_END_ALLOW_THREADS
    }

#if defined(WIN32) && defined(APR_HAS_UNICODE_FS)
    if (wsgi_utf8_to_unicode_path(wfilename, sizeof(wfilename) /
                                  sizeof(apr_wchar_t), filename)) {

        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, WSGI_LOG_ERR(0), r,
                          "mod_wsgi (pid=%d, process='%s', "
                          "application='%s'): Failed to convert '%s' "
                          "to UCS2 filename.", getpid(),
                          process_group, application_group, filename);
        }
        else {
            ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                         "mod_wsgi (pid=%d, process='%s', "
                         "application='%s'): Failed to convert '%s' "
                         "to UCS2 filename.", getpid(),
                         process_group, application_group, filename);
        }
        Py_END_ALLOW_THREADS
        return NULL;
    }

    fp = _wfopen(wfilename, "r");
#else
    fp = fopen(filename, "r");
#endif

    if (!fp) {
        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, WSGI_LOG_ERR(errno), r,
                          "mod_wsgi (pid=%d, process='%s', application='%s'): "
                          "Call to fopen() failed for '%s'.", getpid(),
                          process_group, application_group, filename);
        }
        else {
            ap_log_error(APLOG_MARK, WSGI_LOG_ERR(errno), wsgi_server,
                         "mod_wsgi (pid=%d, process='%s', application='%s'): "
                         "Call to fopen() failed for '%s'.", getpid(),
                         process_group, application_group, filename);
        }
        Py_END_ALLOW_THREADS
        return NULL;
    }

    n = PyParser_SimpleParseFile(fp, filename, Py_file_input);

    fclose(fp);

    if (!n) {
        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, WSGI_LOG_ERR(0), r,
                          "mod_wsgi (pid=%d, process='%s', application='%s'): "
                          "Failed to parse WSGI script file '%s'.", getpid(),
                          process_group, application_group, filename);
        }
        else {
            ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                         "mod_wsgi (pid=%d, process='%s', application='%s'): "
                         "Failed to parse WSGI script file '%s'.", getpid(),
                         process_group, application_group, filename);
        }
        Py_END_ALLOW_THREADS
        return NULL;
    }

    co = (PyObject *)PyNode_Compile(n, filename);
    PyNode_Free(n);

    if (co)
        m = PyImport_ExecCodeModuleEx((char *)name, co, (char *)filename);

    Py_XDECREF(co);

    if (m) {
        PyObject *object = NULL;

        if (!r || strcmp(r->filename, filename)) {
#if AP_SERVER_MAJORVERSION_NUMBER < 2
            struct stat finfo;
            if (stat(filename, &finfo) == -1) {
                object = PyLong_FromLongLong(0);
            }
            else {
                object = PyLong_FromLongLong(finfo.st_mtime);
            }
#else
            apr_finfo_t finfo;
            if (apr_stat(&finfo, filename, APR_FINFO_NORM,
                         pool) != APR_SUCCESS) {
                object = PyLong_FromLongLong(0);
            }
            else {
                object = PyLong_FromLongLong(finfo.mtime);
            }
#endif
        }
        else {
#if AP_SERVER_MAJORVERSION_NUMBER < 2
            object = PyLong_FromLongLong(r->finfo.st_mtime);
#else
            object = PyLong_FromLongLong(r->finfo.mtime);
#endif
        }
        PyModule_AddObject(m, "__mtime__", object);
    }
    else {
        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, WSGI_LOG_ERR(0), r,
                          "mod_wsgi (pid=%d): Target WSGI script '%s' cannot "
                          "be loaded as Python module.", getpid(), filename);
        }
        else {
            ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                         "mod_wsgi (pid=%d): Target WSGI script '%s' cannot "
                         "be loaded as Python module.", getpid(), filename);
        }
        Py_END_ALLOW_THREADS

        wsgi_log_python_error(r, NULL, filename);
    }

    return m;
}