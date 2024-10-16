static int wsgi_execute_dispatch(request_rec *r)
{
    WSGIRequestConfig *config;

    InterpreterObject *interp = NULL;
    PyObject *modules = NULL;
    PyObject *module = NULL;
    char *name = NULL;
    int exists = 0;

    const char *script = NULL;
    const char *group = NULL;

    int status;

    /* Grab request configuration. */

    config = (WSGIRequestConfig *)ap_get_module_config(r->request_config,
                                                       &wsgi_module);

    if (!config->dispatch_script) {
        ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                     "mod_wsgi (pid=%d): Location of WSGI dispatch "
                     "script not provided.", getpid());

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    /*
     * Acquire the desired python interpreter. Once this is done
     * it is safe to start manipulating python objects.
     */

    script = config->dispatch_script->handler_script;
    group = wsgi_server_group(r, config->dispatch_script->application_group);

    interp = wsgi_acquire_interpreter(group);

    if (!interp) {
        ap_log_rerror(APLOG_MARK, WSGI_LOG_CRIT(0), r,
                      "mod_wsgi (pid=%d): Cannot acquire interpreter '%s'.",
                      getpid(), group);

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    /* Calculate the Python module name to be used for script. */

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
     * the last time it was accessed.
     */

    if (module && config->script_reloading) {
        if (wsgi_reload_required(r->pool, r, script, module, NULL)) {
            /*
             * Script file has changed. Only support module
             * reloading for dispatch scripts. Remove the
             * module from the modules dictionary before
             * reloading it again. If code is executing within
             * the module at the time, the callers reference
             * count on the module should ensure it isn't
             * actually destroyed until it is finished.
             */

            Py_DECREF(module);
            module = NULL;

            PyDict_DelItemString(modules, name);
        }
    }

    if (!module) {
        module = wsgi_load_source(r->pool, r, name, exists, script, "", group);
    }

    /* Safe now to release the module lock. */

#if APR_HAS_THREADS
    apr_thread_mutex_unlock(wsgi_module_lock);
#endif

    /* Assume everything will be okay for now. */

    status = OK;

    /* Determine if script exists and execute it. */

    if (module) {
        PyObject *module_dict = NULL;
        PyObject *object = NULL;
        DispatchObject *adapter = NULL;

        module_dict = PyModule_GetDict(module);

        adapter = newDispatchObject(r, config);

        if (adapter) {
            PyObject *vars = NULL;
            PyObject *args = NULL;
            PyObject *method = NULL;

            vars = Dispatch_environ(adapter, group);

            /* First check process_group(). */

#if defined(MOD_WSGI_WITH_DAEMONS)
            object = PyDict_GetItemString(module_dict, "process_group");

            if (object) {
                PyObject *result = NULL;

                if (adapter) {
                    Py_INCREF(object);
                    args = Py_BuildValue("(O)", vars);
                    result = PyEval_CallObject(object, args);
                    Py_DECREF(args);
                    Py_DECREF(object);

                    if (result) {
                        if (result != Py_None) {
                            if (PyString_Check(result)) {
                                const char *s;

                                s = PyString_AsString(result);
                                s = apr_pstrdup(r->pool, s);
                                s = wsgi_process_group(r, s);
                                config->process_group = s;

                                apr_table_setn(r->subprocess_env,
                                               "mod_wsgi.process_group",
                                               config->process_group);
                            }
#if PY_MAJOR_VERSION >= 3
                            else if (PyUnicode_Check(result)) {
                                PyObject *latin_item;
                                latin_item = PyUnicode_AsLatin1String(result);
                                if (!latin_item) {
                                    PyErr_SetString(PyExc_TypeError,
                                                    "Process group must be "
                                                    "a byte string, value "
                                                    "containing non 'latin-1' "
                                                    "characters found");

                                    status = HTTP_INTERNAL_SERVER_ERROR;
                                }
                                else {
                                    const char *s;

                                    Py_DECREF(result);
                                    result = latin_item;

                                    s = PyString_AsString(result);
                                    s = apr_pstrdup(r->pool, s);
                                    s = wsgi_process_group(r, s);
                                    config->process_group = s;

                                    apr_table_setn(r->subprocess_env,
                                                   "mod_wsgi.process_group",
                                                   config->process_group);
                                }
                            }
#endif
                            else {
                                PyErr_SetString(PyExc_TypeError, "Process "
                                                "group must be a byte string");

                                status = HTTP_INTERNAL_SERVER_ERROR;
                            }
                        }

                        Py_DECREF(result);
                    }
                    else
                        status = HTTP_INTERNAL_SERVER_ERROR;
                }
                else
                    Py_DECREF(object);

                object = NULL;
            }
#endif

            /* Now check application_group(). */

            if (status == OK)
                object = PyDict_GetItemString(module_dict, "application_group");

            if (object) {
                PyObject *result = NULL;

                if (adapter) {
                    Py_INCREF(object);
                    args = Py_BuildValue("(O)", vars);
                    result = PyEval_CallObject(object, args);
                    Py_DECREF(args);
                    Py_DECREF(object);

                    if (result) {
                        if (result != Py_None) {
                            if (PyString_Check(result)) {
                                const char *s;

                                s = PyString_AsString(result);
                                s = apr_pstrdup(r->pool, s);
                                s = wsgi_application_group(r, s);
                                config->application_group = s;

                                apr_table_setn(r->subprocess_env,
                                               "mod_wsgi.application_group",
                                               config->application_group);
                            }
#if PY_MAJOR_VERSION >= 3
                            else if (PyUnicode_Check(result)) {
                                PyObject *latin_item;
                                latin_item = PyUnicode_AsLatin1String(result);
                                if (!latin_item) {
                                    PyErr_SetString(PyExc_TypeError,
                                                    "Application group must "
                                                    "be a byte string, value "
                                                    "containing non 'latin-1' "
                                                    "characters found");

                                    status = HTTP_INTERNAL_SERVER_ERROR;
                                }
                                else {
                                    const char *s;

                                    Py_DECREF(result);
                                    result = latin_item;

                                    s = PyString_AsString(result);
                                    s = apr_pstrdup(r->pool, s);
                                    s = wsgi_application_group(r, s);
                                    config->application_group = s;

                                    apr_table_setn(r->subprocess_env,
                                                   "mod_wsgi.application_group",
                                                   config->application_group);
                                }
                            }
#endif
                            else {
                                PyErr_SetString(PyExc_TypeError, "Application "
                                                "group must be a string "
                                                "object");

                                status = HTTP_INTERNAL_SERVER_ERROR;
                            }
                        }

                        Py_DECREF(result);
                    }
                    else
                        status = HTTP_INTERNAL_SERVER_ERROR;
                }
                else
                    Py_DECREF(object);

                object = NULL;
            }

            /* Now check callable_object(). */

            if (status == OK)
                object = PyDict_GetItemString(module_dict, "callable_object");

            if (object) {
                PyObject *result = NULL;

                if (adapter) {
                    Py_INCREF(object);
                    args = Py_BuildValue("(O)", vars);
                    result = PyEval_CallObject(object, args);
                    Py_DECREF(args);
                    Py_DECREF(object);

                    if (result) {
                        if (result != Py_None) {
                            if (PyString_Check(result)) {
                                const char *s;

                                s = PyString_AsString(result);
                                s = apr_pstrdup(r->pool, s);
                                s = wsgi_callable_object(r, s);
                                config->callable_object = s;

                                apr_table_setn(r->subprocess_env,
                                               "mod_wsgi.callable_object",
                                               config->callable_object);
                            }
#if PY_MAJOR_VERSION >= 3
                            else if (PyUnicode_Check(result)) {
                                PyObject *latin_item;
                                latin_item = PyUnicode_AsLatin1String(result);
                                if (!latin_item) {
                                    PyErr_SetString(PyExc_TypeError,
                                                    "Callable object must "
                                                    "be a byte string, value "
                                                    "containing non 'latin-1' "
                                                    "characters found");

                                    status = HTTP_INTERNAL_SERVER_ERROR;
                                }
                                else {
                                    const char *s;

                                    Py_DECREF(result);
                                    result = latin_item;

                                    s = PyString_AsString(result);
                                    s = apr_pstrdup(r->pool, s);
                                    s = wsgi_callable_object(r, s);
                                    config->callable_object = s;

                                    apr_table_setn(r->subprocess_env,
                                                   "mod_wsgi.callable_object",
                                                   config->callable_object);
                                }
                            }
#endif
                            else {
                                PyErr_SetString(PyExc_TypeError, "Callable "
                                                "object must be a string "
                                                "object");

                                status = HTTP_INTERNAL_SERVER_ERROR;
                            }
                        }

                        Py_DECREF(result);
                    }
                    else
                        status = HTTP_INTERNAL_SERVER_ERROR;
                }
                else
                    Py_DECREF(object);

                object = NULL;
            }

            /*
             * Wipe out references to Apache request object
             * held by Python objects, so can detect when an
             * application holds on to the transient Python
             * objects beyond the life of the request and
             * thus raise an exception if they are used.
             */

            adapter->r = NULL;

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

            /* No longer need adapter object. */

            Py_DECREF((PyObject *)adapter);

            /* Log any details of exceptions if execution failed. */

            if (PyErr_Occurred())
                wsgi_log_python_error(r, NULL, script);

            Py_DECREF(vars);
        }
    }

    /* Cleanup and release interpreter, */

    Py_XDECREF(module);

    wsgi_release_interpreter(interp);

    return status;
}