static PyObject *wsgi_signal_intercept(PyObject *self, PyObject *args)
{
    PyObject *h = NULL;
    int n = 0;

    PyObject *m = NULL;

    if (!PyArg_ParseTuple(args, "iO:signal", &n, &h))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
    ap_log_error(APLOG_MARK, WSGI_LOG_WARNING(0), wsgi_server,
                 "mod_wsgi (pid=%d): Callback registration for "
                 "signal %d ignored.", getpid(), n);
    Py_END_ALLOW_THREADS

    m = PyImport_ImportModule("traceback");

    if (m) {
        PyObject *d = NULL;
        PyObject *o = NULL;
        d = PyModule_GetDict(m);
        o = PyDict_GetItemString(d, "print_stack");
        if (o) {
            PyObject *log = NULL;
            PyObject *args = NULL;
            PyObject *result = NULL;
            Py_INCREF(o);
            log = newLogObject(NULL, APLOG_WARNING, NULL);
            args = Py_BuildValue("(OOO)", Py_None, Py_None, log);
            result = PyEval_CallObject(o, args);
            Py_XDECREF(result);
            Py_DECREF(args);
            Py_DECREF(log);
            Py_DECREF(o);
        }
    }

    Py_XDECREF(m);

    Py_INCREF(h);

    return h;
}