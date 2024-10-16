static void wsgi_log_python_error(request_rec *r, PyObject *log,
                                  const char *filename)
{
    PyObject *m = NULL;
    PyObject *result = NULL;

    PyObject *type = NULL;
    PyObject *value = NULL;
    PyObject *traceback = NULL;

    PyObject *xlog = NULL;

    if (!PyErr_Occurred())
        return;

    if (!log) {
        PyErr_Fetch(&type, &value, &traceback);

        xlog = newLogObject(r, APLOG_ERR, NULL);

        log = xlog;

        PyErr_Restore(type, value, traceback);

        type = NULL;
        value = NULL;
        traceback = NULL;
    }

    if (PyErr_ExceptionMatches(PyExc_SystemExit)) {
        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, WSGI_LOG_ERR(0), r,
                          "mod_wsgi (pid=%d): SystemExit exception raised by "
                          "WSGI script '%s' ignored.", getpid(), filename);
        }
        else {
            ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                          "mod_wsgi (pid=%d): SystemExit exception raised by "
                          "WSGI script '%s' ignored.", getpid(), filename);
        }
        Py_END_ALLOW_THREADS
    }
    else {
        Py_BEGIN_ALLOW_THREADS
        if (r) {
            ap_log_rerror(APLOG_MARK, WSGI_LOG_ERR(0), r,
                          "mod_wsgi (pid=%d): Exception occurred processing "
                          "WSGI script '%s'.", getpid(), filename);
        }
        else {
            ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                          "mod_wsgi (pid=%d): Exception occurred processing "
                          "WSGI script '%s'.", getpid(), filename);
        }
        Py_END_ALLOW_THREADS
    }

    PyErr_Fetch(&type, &value, &traceback);
    PyErr_NormalizeException(&type, &value, &traceback);

    if (!value) {
        value = Py_None;
        Py_INCREF(value);
    }

    if (!traceback) {
        traceback = Py_None;
        Py_INCREF(traceback);
    }

    m = PyImport_ImportModule("traceback");

    if (m) {
        PyObject *d = NULL;
        PyObject *o = NULL;
        d = PyModule_GetDict(m);
        o = PyDict_GetItemString(d, "print_exception");
        if (o) {
            PyObject *args = NULL;
            Py_INCREF(o);
            args = Py_BuildValue("(OOOOO)", type, value, traceback,
                                 Py_None, log);
            result = PyEval_CallObject(o, args);
            Py_DECREF(args);
            Py_DECREF(o);
        }
    }

    if (!result) {
        /*
         * If can't output exception and traceback then
         * use PyErr_Print to dump out details of the
         * exception. For SystemExit though if we do
         * that the process will actually be terminated
         * so can only clear the exception information
         * and keep going.
         */

        PyErr_Restore(type, value, traceback);

        if (!PyErr_ExceptionMatches(PyExc_SystemExit)) {
            PyErr_Print();
            PyErr_Clear();
        }
        else {
            PyErr_Clear();
        }
    }
    else {
        Py_XDECREF(type);
        Py_XDECREF(value);
        Py_XDECREF(traceback);
    }

    Py_XDECREF(result);

    Py_XDECREF(m);

    Py_XDECREF(xlog);
}