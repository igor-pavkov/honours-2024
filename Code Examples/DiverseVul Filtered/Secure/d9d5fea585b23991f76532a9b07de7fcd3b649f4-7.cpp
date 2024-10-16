static int wsgi_reload_required(apr_pool_t *pool, request_rec *r,
                                const char *filename, PyObject *module,
                                const char *resource)
{
    PyObject *dict = NULL;
    PyObject *object = NULL;
    apr_time_t mtime = 0;

    dict = PyModule_GetDict(module);
    object = PyDict_GetItemString(dict, "__mtime__");

    if (object) {
        mtime = PyLong_AsLongLong(object);

        if (!r || strcmp(r->filename, filename)) {
#if AP_SERVER_MAJORVERSION_NUMBER < 2
            struct stat finfo;
            if (stat(filename, &finfo) == -1) {
                return 1;
            }
            else if (mtime != finfo.st_mtime) {
                return 1;
            }
#else
            apr_finfo_t finfo;
            if (apr_stat(&finfo, filename, APR_FINFO_NORM,
                         pool) != APR_SUCCESS) {
                return 1;
            }
            else if (mtime != finfo.mtime) {
                return 1;
            }
#endif
        }
        else {
#if AP_SERVER_MAJORVERSION_NUMBER < 2
            if (mtime != r->finfo.st_mtime)
                return 1;
#else
            if (mtime != r->finfo.mtime)
                return 1;
#endif
        }
    }
    else
        return 1;

    if (resource) {
        PyObject *dict = NULL;
        PyObject *object = NULL;

        dict = PyModule_GetDict(module);
        object = PyDict_GetItemString(dict, "reload_required");

        if (object) {
            PyObject *args = NULL;
            PyObject *result = NULL;

            Py_INCREF(object);
            args = Py_BuildValue("(s)", resource);
            result = PyEval_CallObject(object, args);
            Py_DECREF(args);
            Py_DECREF(object);

            if (result && PyObject_IsTrue(result)) {
                Py_DECREF(result);

                return 1;
            }

            if (PyErr_Occurred())
                wsgi_log_python_error(r, NULL, filename);

            Py_XDECREF(result);
        }
    }

    return 0;
}