static PyObject *Adapter_environ(AdapterObject *self)
{
    request_rec *r = NULL;

    PyObject *vars = NULL;
    PyObject *object = NULL;

    const apr_array_header_t *head = NULL;
    const apr_table_entry_t *elts = NULL;

    int i = 0;

    const char *scheme = NULL;

    /* Create the WSGI environment dictionary. */

    vars = PyDict_New();

    /* Merge the CGI environment into the WSGI environment. */

    r = self->r;

    head = apr_table_elts(r->subprocess_env);
    elts = (apr_table_entry_t *)head->elts;

    for (i = 0; i < head->nelts; ++i) {
        if (elts[i].key) {
            if (elts[i].val) {
#if PY_MAJOR_VERSION >= 3
                if (!strcmp(elts[i].val, "DOCUMENT_ROOT")) {
                    object = PyUnicode_Decode(elts[i].val, strlen(elts[i].val),
                                             Py_FileSystemDefaultEncoding,
                                             "surrogateescape");
                }
                else if (!strcmp(elts[i].val, "SCRIPT_FILENAME")) {
                    object = PyUnicode_Decode(elts[i].val, strlen(elts[i].val),
                                             Py_FileSystemDefaultEncoding,
                                             "surrogateescape");
                }
                else {
                    object = PyUnicode_DecodeLatin1(elts[i].val,
                                                    strlen(elts[i].val), NULL);
                }
#else
                object = PyString_FromString(elts[i].val);
#endif
                PyDict_SetItemString(vars, elts[i].key, object);
                Py_DECREF(object);
            }
            else
                PyDict_SetItemString(vars, elts[i].key, Py_None);
        }
    }

    PyDict_DelItemString(vars, "PATH");

    /* Now setup all the WSGI specific environment values. */

    object = Py_BuildValue("(ii)", 1, 0);
    PyDict_SetItemString(vars, "wsgi.version", object);
    Py_DECREF(object);

    object = PyBool_FromLong(wsgi_multithread);
    PyDict_SetItemString(vars, "wsgi.multithread", object);
    Py_DECREF(object);

    object = PyBool_FromLong(wsgi_multiprocess);
    PyDict_SetItemString(vars, "wsgi.multiprocess", object);
    Py_DECREF(object);

#if defined(MOD_WSGI_WITH_DAEMONS)
    if (wsgi_daemon_process) {
        if (wsgi_daemon_process->group->threads == 1 &&
            wsgi_daemon_process->group->maximum_requests == 1) {
            PyDict_SetItemString(vars, "wsgi.run_once", Py_True);
        }
        else
            PyDict_SetItemString(vars, "wsgi.run_once", Py_False);
    }
    else
        PyDict_SetItemString(vars, "wsgi.run_once", Py_False);
#else
    PyDict_SetItemString(vars, "wsgi.run_once", Py_False);
#endif

    scheme = apr_table_get(r->subprocess_env, "HTTPS");

    if (scheme && (!strcasecmp(scheme, "On") || !strcmp(scheme, "1"))) {
#if PY_MAJOR_VERSION >= 3
        object = PyUnicode_FromString("https");
#else
        object = PyString_FromString("https");
#endif
        PyDict_SetItemString(vars, "wsgi.url_scheme", object);
        Py_DECREF(object);
    }
    else {
#if PY_MAJOR_VERSION >= 3
        object = PyUnicode_FromString("http");
#else
        object = PyString_FromString("http");
#endif
        PyDict_SetItemString(vars, "wsgi.url_scheme", object);
        Py_DECREF(object);
    }

    /*
     * We remove the HTTPS variable because WSGI compliant
     * applications shouldn't rely on it. Instead they should
     * use wsgi.url_scheme. We do this even if SetEnv was
     * used to set HTTPS from Apache configuration. That is
     * we convert it into the correct variable and remove the
     * original.
     */

    if (scheme)
        PyDict_DelItemString(vars, "HTTPS");

    /*
     * Setup log object for WSGI errors. Don't decrement
     * reference to log object as keep reference to it.
     */

    object = (PyObject *)self->log;
    PyDict_SetItemString(vars, "wsgi.errors", object);

    /* Setup input object for request content. */

    object = (PyObject *)self->input;
    PyDict_SetItemString(vars, "wsgi.input", object);

    /* Setup file wrapper object for efficient file responses. */

    object = PyObject_GetAttrString((PyObject *)self, "file_wrapper");
    PyDict_SetItemString(vars, "wsgi.file_wrapper", object);
    Py_DECREF(object);

    /* Add mod_wsgi version information. */

    object = Py_BuildValue("(ii)", MOD_WSGI_MAJORVERSION_NUMBER,
                           MOD_WSGI_MINORVERSION_NUMBER);
    PyDict_SetItemString(vars, "mod_wsgi.version", object);
    Py_DECREF(object);

    /*
     * If Apache extensions are enabled and running in embedded
     * mode add a CObject reference to the Apache request_rec
     * structure instance.
     */

    if (!wsgi_daemon_pool && self->config->pass_apache_request) {
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 2
        object = PyCapsule_New(self->r, 0, 0);
#else
        object = PyCObject_FromVoidPtr(self->r, 0);
#endif
        PyDict_SetItemString(vars, "apache.request_rec", object);
        Py_DECREF(object);
    }

    /*
     * Extensions for accessing SSL certificate information from
     * mod_ssl when in use.
     */

#if 0
#if AP_SERVER_MAJORVERSION_NUMBER >= 2
    if (!wsgi_daemon_pool) {
        object = PyObject_GetAttrString((PyObject *)self, "ssl_is_https");
        PyDict_SetItemString(vars, "mod_ssl.is_https", object);
        Py_DECREF(object);

        object = PyObject_GetAttrString((PyObject *)self, "ssl_var_lookup");
        PyDict_SetItemString(vars, "mod_ssl.var_lookup", object);
        Py_DECREF(object);
    }
#endif
#endif

    return vars;
}