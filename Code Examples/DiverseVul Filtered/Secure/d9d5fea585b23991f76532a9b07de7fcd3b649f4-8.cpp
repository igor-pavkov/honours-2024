static apr_status_t wsgi_python_term()
{
    PyInterpreterState *interp = NULL;
    PyThreadState *tstate = NULL;

    PyObject *module = NULL;

    ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                 "mod_wsgi (pid=%d): Terminating Python.", getpid());

    /*
     * We should be executing in the main thread again at this
     * point but without the GIL, so simply restore the original
     * thread state for that thread that we remembered when we
     * initialised the interpreter.
     */

    PyEval_AcquireThread(wsgi_main_tstate);

    /*
     * Work around bug in Python 3.X whereby it will crash if
     * atexit imported into sub interpreter, but never imported
     * into main interpreter before calling Py_Finalize(). We
     * perform an import of atexit module and it as side effect
     * must be performing required initialisation.
     */

    module = PyImport_ImportModule("atexit");
    Py_XDECREF(module);

    /*
     * In Python 2.6.5 and Python 3.1.2 the shutdown of
     * threading was moved back into Py_Finalize() for the main
     * Python interpreter. Because we shutting down threading
     * ourselves, the second call results in errors being logged
     * when Py_Finalize() is called and the shutdown function
     * called a second time. The errors don't indicate any real
     * problem and the threading module ignores them anyway.
     * Whether we are using Python with this changed behaviour
     * can only be checked by looking at run time version.
     * Rather than try and add a dynamic check, create a fake
     * 'dummy_threading' module as the presence of that shuts up
     * the messages. It doesn't matter that the rest of the
     * shutdown function still runs as everything is already
     * stopped so doesn't do anything.
     */

    if (!PyImport_AddModule("dummy_threading"))
        PyErr_Clear();

    Py_Finalize();

    wsgi_python_initialized = 0;

    ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                 "mod_wsgi (pid=%d): Python has shutdown.", getpid());

    return APR_SUCCESS;
}