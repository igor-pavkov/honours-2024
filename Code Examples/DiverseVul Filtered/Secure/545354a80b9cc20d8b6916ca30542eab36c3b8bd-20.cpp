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

    PyObject *transaction = NULL;

#if defined(WIN32) && defined(APR_HAS_UNICODE_FS)
    apr_wchar_t wfilename[APR_PATH_MAX];
#endif

    if (exists) {
        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r,
                          "mod_wsgi (pid=%d, process='%s', application='%s'): "
                          "Reloading WSGI script '%s'.", getpid(),
                          process_group, application_group, filename);
        }
        else {
            ap_log_error(APLOG_MARK, APLOG_INFO, 0, wsgi_server,
                         "mod_wsgi (pid=%d, process='%s', application='%s'): "
                         "Reloading WSGI script '%s'.", getpid(),
                         process_group, application_group, filename);
        }
        Py_END_ALLOW_THREADS
    }
    else {
        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r,
                          "mod_wsgi (pid=%d, process='%s', application='%s'): "
                          "Loading WSGI script '%s'.", getpid(),
                          process_group, application_group, filename);
        }
        else {
            ap_log_error(APLOG_MARK, APLOG_INFO, 0, wsgi_server,
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
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                          "mod_wsgi (pid=%d, process='%s', "
                          "application='%s'): Failed to convert '%s' "
                          "to UCS2 filename.", getpid(),
                          process_group, application_group, filename);
        }
        else {
            ap_log_error(APLOG_MARK, APLOG_ERR, 0, wsgi_server,
                         "mod_wsgi (pid=%d, process='%s', "
                         "application='%s'): Failed to convert '%s' "
                         "to UCS2 filename.", getpid(),
                         process_group, application_group, filename);
        }
        Py_END_ALLOW_THREADS
        return NULL;
    }

    fp = _wfopen(wfilename, L"r");
#else
    fp = fopen(filename, "r");
#endif

    if (!fp) {
        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, errno, r,
                          "mod_wsgi (pid=%d, process='%s', application='%s'): "
                          "Call to fopen() failed for '%s'.", getpid(),
                          process_group, application_group, filename);
        }
        else {
            ap_log_error(APLOG_MARK, APLOG_ERR, errno, wsgi_server,
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
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                          "mod_wsgi (pid=%d, process='%s', application='%s'): "
                          "Failed to parse WSGI script file '%s'.", getpid(),
                          process_group, application_group, filename);
        }
        else {
            ap_log_error(APLOG_MARK, APLOG_ERR, 0, wsgi_server,
                         "mod_wsgi (pid=%d, process='%s', application='%s'): "
                         "Failed to parse WSGI script file '%s'.", getpid(),
                         process_group, application_group, filename);
        }
        Py_END_ALLOW_THREADS
        return NULL;
    }

    if (wsgi_newrelic_config_file) {
        PyObject *module = NULL;

        PyObject *application = NULL;


        module = PyImport_ImportModule("newrelic.api.application");

        if (module) {
            PyObject *dict = NULL;
            PyObject *object = NULL;

            dict = PyModule_GetDict(module);
            object = PyDict_GetItemString(dict, "application");

            Py_INCREF(object);
            application = PyObject_CallFunctionObjArgs(object, NULL);
            Py_DECREF(object);

            Py_DECREF(module);
            module = NULL;

            if (!application)
                PyErr_Clear();
        }
        else
            PyErr_Clear();

        if (application)
            module = PyImport_ImportModule("newrelic.api.background_task");

        if (module) {
            PyObject *dict = NULL;
            PyObject *object = NULL;

            dict = PyModule_GetDict(module);
            object = PyDict_GetItemString(dict, "BackgroundTask");

            if (object) {
                PyObject *args = NULL;

                Py_INCREF(object);

                args = Py_BuildValue("(Oss)", application, filename,
                                     "Script/Import");
                transaction = PyObject_Call(object, args, NULL);

                if (!transaction)
                    PyErr_WriteUnraisable(object);

                Py_DECREF(args);
                Py_DECREF(object);

                if (transaction) {
                    PyObject *result = NULL;

                    object = PyObject_GetAttrString(
                            transaction, "__enter__");
                    args = PyTuple_Pack(0);
                    result = PyObject_Call(object, args, NULL);

                    if (!result)
                        PyErr_WriteUnraisable(object);

                    Py_XDECREF(result);
                    Py_DECREF(object);
                }
            }

            Py_DECREF(module);
        }
        else
            PyErr_Print();

        Py_XDECREF(application);
    }
    else
        PyErr_Clear();

    co = (PyObject *)PyNode_Compile(n, filename);
    PyNode_Free(n);

    if (co)
        m = PyImport_ExecCodeModuleEx((char *)name, co, (char *)filename);

    Py_XDECREF(co);

    if (wsgi_newrelic_config_file) {
        if (transaction) {
            PyObject *object;

            object = PyObject_GetAttrString(transaction, "__exit__");

            if (m) {
                PyObject *args = NULL;
                PyObject *result = NULL;

                args = PyTuple_Pack(3, Py_None, Py_None, Py_None);
                result = PyObject_Call(object, args, NULL);

                if (!result)
                    PyErr_WriteUnraisable(object);
                else
                    Py_DECREF(result);

                Py_DECREF(args);
            }
            else {
                PyObject *args = NULL;
                PyObject *result = NULL;

                PyObject *type = NULL;
                PyObject *value = NULL;
                PyObject *traceback = NULL;

                PyErr_Fetch(&type, &value, &traceback);

                if (!value) {
                    value = Py_None;
                    Py_INCREF(value);
                }

                if (!traceback) {
                    traceback = Py_None;
                    Py_INCREF(traceback);
                }

                PyErr_NormalizeException(&type, &value, &traceback);

                args = PyTuple_Pack(3, type, value, traceback);
                result = PyObject_Call(object, args, NULL);

                if (!result)
                    PyErr_WriteUnraisable(object);
                else
                    Py_DECREF(result);

                Py_DECREF(args);

                PyErr_Restore(type, value, traceback);
            }

            Py_DECREF(object);

            Py_DECREF(transaction);
        }
    }

    if (m) {
        PyObject *object = NULL;

        if (!r || strcmp(r->filename, filename)) {
            apr_finfo_t finfo;
            if (apr_stat(&finfo, filename, APR_FINFO_NORM,
                         pool) != APR_SUCCESS) {
                object = PyLong_FromLongLong(0);
            }
            else {
                object = PyLong_FromLongLong(finfo.mtime);
            }
        }
        else {
            object = PyLong_FromLongLong(r->finfo.mtime);
        }
        PyModule_AddObject(m, "__mtime__", object);
    }
    else {
        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                          "mod_wsgi (pid=%d): Target WSGI script '%s' cannot "
                          "be loaded as Python module.", getpid(), filename);
        }
        else {
            ap_log_error(APLOG_MARK, APLOG_ERR, 0, wsgi_server,
                         "mod_wsgi (pid=%d): Target WSGI script '%s' cannot "
                         "be loaded as Python module.", getpid(), filename);
        }
        Py_END_ALLOW_THREADS

        wsgi_log_python_error(r, NULL, filename);
    }

    return m;
}