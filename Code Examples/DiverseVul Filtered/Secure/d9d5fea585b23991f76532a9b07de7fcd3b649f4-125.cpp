static apr_status_t wsgi_python_child_cleanup(void *data)
#endif
{
    PyObject *interp = NULL;

    /* In a multithreaded MPM must protect table. */

#if APR_HAS_THREADS
    apr_thread_mutex_lock(wsgi_interp_lock);
#endif

    /*
     * We should be executing in the main thread again at this
     * point but without the GIL, so simply restore the original
     * thread state for that thread that we remembered when we
     * initialised the interpreter.
     */

    PyEval_AcquireThread(wsgi_main_tstate);

    /*
     * Extract a handle to the main Python interpreter from
     * interpreters dictionary as want to process that one last.
     */

    interp = PyDict_GetItemString(wsgi_interpreters, "");
    Py_INCREF(interp);

    /*
     * Remove all items from interpreters dictionary. This will
     * have side affect of calling any exit functions and
     * destroying interpreters we own.
     */

    ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                 "mod_wsgi (pid=%d): Destroying interpreters.", getpid());

    PyDict_Clear(wsgi_interpreters);

#if APR_HAS_THREADS
    apr_thread_mutex_unlock(wsgi_interp_lock);
#endif

    /*
     * Now we decrement reference on handle for main Python
     * interpreter. This only causes exit functions to be called
     * and doesn't result in interpreter being destroyed as we
     * we didn't previously mark ourselves as the owner of the
     * interpreter. Note that when Python as a whole is later
     * being destroyed it will also call exit functions, but by
     * then the exit function registrations have been removed
     * and so they will not actually be run a second time.
     */

    Py_DECREF(interp);

    /*
     * The code which performs actual shutdown of the main
     * interpreter expects to be called without the GIL, so
     * we release it here again.
     */

    PyEval_ReleaseThread(wsgi_main_tstate);

    /*
     * Destroy Python itself including the main interpreter.
     * If mod_python is being loaded it is left to mod_python to
     * destroy Python, although it currently doesn't do so.
     */

    if (wsgi_python_initialized)
        wsgi_python_term();

#if AP_SERVER_MAJORVERSION_NUMBER >= 2
    return APR_SUCCESS;
#endif
}