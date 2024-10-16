static int wsgi_execute_script(request_rec *r)
{
    WSGIRequestConfig *config = NULL;

    InterpreterObject *interp = NULL;
    PyObject *modules = NULL;
    PyObject *module = NULL;
    const char *script = NULL;
    const char *name = NULL;
    int exists = 0;

    int status;

    /* Grab request configuration. */

    config = (WSGIRequestConfig *)ap_get_module_config(r->request_config,
                                                       &wsgi_module);

    /*
     * Acquire the desired python interpreter. Once this is done
     * it is safe to start manipulating python objects.
     */

    interp = wsgi_acquire_interpreter(config->application_group);

    if (!interp) {
        ap_log_rerror(APLOG_MARK, WSGI_LOG_CRIT(0), r,
                      "mod_wsgi (pid=%d): Cannot acquire interpreter '%s'.",
                      getpid(), config->application_group);

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    /* Calculate the Python module name to be used for script. */

    if (config->handler_script && *config->handler_script)
        script = config->handler_script;
    else
        script = r->filename;

    name = wsgi_module_name(r->pool, script);

    /*
     * Use a lock around the check to see if the module is
     * already loaded and the import of the module to prevent
     * two request handlers trying to import the module at the
     * same time.
     */

#if APR_HAS_THREADS
    Py_BEGIN_ALLOW_THREADS
    apr_thread_mutex_lock(wsgi_module_lock);
    Py_END_ALLOW_THREADS
#endif

    modules = PyImport_GetModuleDict();
    module = PyDict_GetItemString(modules, name);

    Py_XINCREF(module);

    if (module)
        exists = 1;

    /*
     * If script reloading is enabled and the module for it has
     * previously been loaded, see if it has been modified since
     * the last time it was accessed. For a handler script will
     * also see if it contains a custom function for determining
     * if a reload should be performed.
     */

    if (module && config->script_reloading) {
        if (wsgi_reload_required(r->pool, r, script, module, r->filename)) {
            /*
             * Script file has changed. Discard reference to
             * loaded module and work out what action we are
             * supposed to take. Choices are process reloading
             * and module reloading. Process reloading cannot be
             * performed unless a daemon process is being used.
             */

            Py_DECREF(module);
            module = NULL;

#if defined(MOD_WSGI_WITH_DAEMONS)
            if (*config->process_group) {
                /*
                 * Need to restart the daemon process. We bail
                 * out on the request process here, sending back
                 * a special response header indicating that
                 * process is being restarted and that remote
                 * end should abandon connection and attempt to
                 * reconnect again. We also need to signal this
                 * process so it will actually shutdown. The
                 * process supervisor code will ensure that it
                 * is restarted.
                 */

                Py_BEGIN_ALLOW_THREADS
                ap_log_rerror(APLOG_MARK, WSGI_LOG_INFO(0), r,
                             "mod_wsgi (pid=%d): Force restart of "
                             "process '%s'.", getpid(),
                             config->process_group);
                Py_END_ALLOW_THREADS

#if APR_HAS_THREADS
                apr_thread_mutex_unlock(wsgi_module_lock);
#endif

                wsgi_release_interpreter(interp);

                r->status = HTTP_INTERNAL_SERVER_ERROR;
                r->status_line = "200 Rejected";

                wsgi_daemon_shutdown++;
                kill(getpid(), SIGINT);

                return OK;
            }
            else {
                /*
                 * Need to reload just the script module. Remove
                 * the module from the modules dictionary before
                 * reloading it again. If code is executing
                 * within the module at the time, the callers
                 * reference count on the module should ensure
                 * it isn't actually destroyed until it is
                 * finished.
                 */

                PyDict_DelItemString(modules, name);
            }
#else
            /*
             * Need to reload just the script module. Remove
             * the module from the modules dictionary before
             * reloading it again. If code is executing
             * within the module at the time, the callers
             * reference count on the module should ensure
             * it isn't actually destroyed until it is
             * finished.
             */

            PyDict_DelItemString(modules, name);
#endif
        }
    }

    /*
     * When process reloading is in use need to indicate
     * that request content should now be sent through.
     * This is done by writing a special response header
     * directly out onto the appropriate network output
     * filter. The special response is picked up by
     * remote end and data will then be sent.
     */

#if defined(MOD_WSGI_WITH_DAEMONS)
    if (*config->process_group) {
        ap_filter_t *filters;
        apr_bucket_brigade *bb;
        apr_bucket *b;

        const char *data = "Status: 200 Continue\r\n\r\n";
        int length = strlen(data);

        filters = r->output_filters;
        while (filters && filters->frec->ftype != AP_FTYPE_NETWORK) {
            filters = filters->next;
        }

        bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);

        b = apr_bucket_transient_create(data, length,
                                        r->connection->bucket_alloc);
        APR_BRIGADE_INSERT_TAIL(bb, b);

        b = apr_bucket_flush_create(r->connection->bucket_alloc);
        APR_BRIGADE_INSERT_TAIL(bb, b);

        /*
         * This should always work, so ignore any errors
         * from passing the brigade to the network
         * output filter. If there are are problems they
         * will be picked up further down in processing
         * anyway.
         */

        ap_pass_brigade(filters, bb);
    }
#endif

    /* Load module if not already loaded. */

    if (!module) {
        module = wsgi_load_source(r->pool, r, name, exists, script,
                                  config->process_group,
                                  config->application_group);
    }

    /* Safe now to release the module lock. */

#if APR_HAS_THREADS
    apr_thread_mutex_unlock(wsgi_module_lock);
#endif

    /* Assume an internal server error unless everything okay. */

    status = HTTP_INTERNAL_SERVER_ERROR;

    /* Determine if script exists and execute it. */

    if (module) {
        PyObject *module_dict = NULL;
        PyObject *object = NULL;

        module_dict = PyModule_GetDict(module);
        object = PyDict_GetItemString(module_dict, config->callable_object);

        if (object) {
            AdapterObject *adapter = NULL;
            adapter = newAdapterObject(r);

            if (adapter) {
                PyObject *method = NULL;
                PyObject *args = NULL;

                Py_INCREF(object);
                status = Adapter_run(adapter, object);
                Py_DECREF(object);

                /*
                 * Wipe out references to Apache request object
                 * held by Python objects, so can detect when an
                 * application holds on to the transient Python
                 * objects beyond the life of the request and
                 * thus raise an exception if they are used.
                 */

                adapter->r = NULL;
                adapter->input->r = NULL;

                /* Close the log object so data is flushed. */

                method = PyObject_GetAttrString(adapter->log, "close");

                if (!method) {
                    PyErr_Format(PyExc_AttributeError,
                                 "'%s' object has no attribute 'close'",
                                 adapter->log->ob_type->tp_name);
                }
                else {
                    args = PyTuple_New(0);
                    object = PyEval_CallObject(method, args);
                    Py_DECREF(args);
                }

                Py_XDECREF(object);
                Py_XDECREF(method);

#if defined(MOD_WSGI_WITH_BUCKETS)
                adapter->bb = NULL;
#endif
            }

            Py_XDECREF((PyObject *)adapter);
        }
        else {
            Py_BEGIN_ALLOW_THREADS
            ap_log_rerror(APLOG_MARK, WSGI_LOG_ERR(0), r,
                          "mod_wsgi (pid=%d): Target WSGI script '%s' does "
                          "not contain WSGI application '%s'.",
                          getpid(), script, config->callable_object);
            Py_END_ALLOW_THREADS

            status = HTTP_NOT_FOUND;
        }
    }

    /* Log any details of exceptions if execution failed. */

    if (PyErr_Occurred())
        wsgi_log_python_error(r, NULL, r->filename);

    /* Cleanup and release interpreter, */

    Py_XDECREF(module);

    wsgi_release_interpreter(interp);

    return status;
}