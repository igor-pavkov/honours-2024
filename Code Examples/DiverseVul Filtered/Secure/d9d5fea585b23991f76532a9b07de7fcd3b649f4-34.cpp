static PyObject *Dispatch_environ(DispatchObject *self, const char *group)
{
    request_rec *r = NULL;

    PyObject *vars = NULL;
    PyObject *object = NULL;

    const apr_array_header_t *head = NULL;
    const apr_table_entry_t *elts = NULL;

    int i = 0;

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
                object = PyUnicode_DecodeLatin1(elts[i].val,
                                                strlen(elts[i].val), NULL);
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

    /*
     * Need to override process and application group as they
     * are set to the default target, where as we need to set
     * them to context dispatch script is run in. Also need
     * to remove callable object reference.
     */

#if PY_MAJOR_VERSION >= 3
    object = PyUnicode_FromString("");
#else
    object = PyString_FromString("");
#endif
    PyDict_SetItemString(vars, "mod_wsgi.process_group", object);
    Py_DECREF(object);

#if PY_MAJOR_VERSION >= 3
    object = PyUnicode_DecodeLatin1(group, strlen(group), NULL);
#else
    object = PyString_FromString(group);
#endif
    PyDict_SetItemString(vars, "mod_wsgi.application_group", object);
    Py_DECREF(object);

    PyDict_DelItemString(vars, "mod_wsgi.callable_object");

    /*
     * Setup log object for WSGI errors. Don't decrement
     * reference to log object as keep reference to it.
     */

    object = (PyObject *)self->log;
    PyDict_SetItemString(vars, "wsgi.errors", object);

    /*
     * If Apache extensions are enabled add a CObject reference
     * to the Apache request_rec structure instance.
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
    object = PyObject_GetAttrString((PyObject *)self, "ssl_is_https");
    PyDict_SetItemString(vars, "mod_ssl.is_https", object);
    Py_DECREF(object);

    object = PyObject_GetAttrString((PyObject *)self, "ssl_var_lookup");
    PyDict_SetItemString(vars, "mod_ssl.var_lookup", object);
    Py_DECREF(object);
#endif
#endif

    return vars;
}