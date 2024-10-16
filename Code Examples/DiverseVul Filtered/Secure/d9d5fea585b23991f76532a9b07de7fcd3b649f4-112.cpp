static InterpreterObject *wsgi_acquire_interpreter(const char *name)
{
    PyThreadState *tstate = NULL;
    PyInterpreterState *interp = NULL;
    InterpreterObject *handle = NULL;

    PyGILState_STATE state;

    /*
     * In a multithreaded MPM must protect the
     * interpreters table. This lock is only needed to
     * avoid a secondary thread coming in and creating
     * the same interpreter if Python releases the GIL
     * when an interpreter is being created.
     */

#if APR_HAS_THREADS
    apr_thread_mutex_lock(wsgi_interp_lock);
#endif

    /*
     * This function should never be called when the
     * Python GIL is held, so need to acquire it. Even
     * though we may need to work with a sub
     * interpreter, we need to acquire GIL against main
     * interpreter first to work with interpreter
     * dictionary.
     */

    state = PyGILState_Ensure();

    /*
     * Check if already have interpreter instance and
     * if not need to create one.
     */

    handle = (InterpreterObject *)PyDict_GetItemString(wsgi_interpreters,
                                                       name);

    if (!handle) {
        handle = newInterpreterObject(name);

        if (!handle) {
            ap_log_error(APLOG_MARK, WSGI_LOG_CRIT(0), wsgi_server,
                         "mod_wsgi (pid=%d): Cannot create interpreter '%s'.",
                         getpid(), name);

            PyErr_Print();
            PyErr_Clear();

            PyGILState_Release(state);

#if APR_HAS_THREADS
            apr_thread_mutex_unlock(wsgi_interp_lock);
#endif
            return NULL;
        }

        PyDict_SetItemString(wsgi_interpreters, name, (PyObject *)handle);
    }
    else
        Py_INCREF(handle);

    interp = handle->interp;

    /*
     * Create new thread state object. We should only be
     * getting called where no current active thread
     * state, so no need to remember the old one. When
     * working with the main Python interpreter always
     * use the simplified API for GIL locking so any
     * extension modules which use that will still work.
     */

    PyGILState_Release(state);

#if APR_HAS_THREADS
    apr_thread_mutex_unlock(wsgi_interp_lock);
#endif

    if (*name) {
#if APR_HAS_THREADS
        int thread_id = 0;
        int *thread_handle = NULL;

        apr_threadkey_private_get((void**)&thread_handle, wsgi_thread_key);

        if (!thread_handle) {
            thread_id = wsgi_thread_count++;
            thread_handle = (int*)apr_pmemdup(wsgi_server->process->pool,
                                              &thread_id, sizeof(thread_id));
            apr_threadkey_private_set(thread_handle, wsgi_thread_key);
        }
        else {
            thread_id = *thread_handle;
        }

        tstate = apr_hash_get(handle->tstate_table, &thread_id,
                              sizeof(thread_id));

        if (!tstate) {
            tstate = PyThreadState_New(interp);

            if (wsgi_server_config->verbose_debugging) {
                ap_log_error(APLOG_MARK, WSGI_LOG_DEBUG(0), wsgi_server,
                             "mod_wsgi (pid=%d): Create thread state for "
                             "thread %d against interpreter '%s'.", getpid(),
                             thread_id, handle->name);
            }

            apr_hash_set(handle->tstate_table, thread_handle,
                         sizeof(*thread_handle), tstate);
        }
#else
        tstate = handle->tstate;
#endif

        PyEval_AcquireThread(tstate);
    }
    else {
        PyGILState_Ensure();

        /*
         * When simplified GIL state API is used, the thread
         * local data only persists for the extent of the top
         * level matching ensure/release calls. We want to
         * extend lifetime of the thread local data beyond
         * that, retaining it for all requests within the one
         * thread for the life of the process. To do that we
         * need to artificially increment the reference count
         * for the associated thread state object.
         */

        tstate = PyThreadState_Get();
        if (tstate && tstate->gilstate_counter == 1)
            tstate->gilstate_counter++;
    }

    return handle;
}