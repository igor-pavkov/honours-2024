static PyObject *Adapter_start_response(AdapterObject *self, PyObject *args)
{
    const char *status = NULL;
    PyObject *headers = NULL;
    PyObject *exc_info = NULL;

    PyObject *item = NULL;
    PyObject *latin_item = NULL;

    char* value = NULL;

    if (!self->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "OO|O:start_response",
        &item, &headers, &exc_info)) {
        return NULL;
    }

#if PY_MAJOR_VERSION >= 3
    if (PyUnicode_Check(item)) {
        latin_item = PyUnicode_AsLatin1String(item);
        if (!latin_item) {
            PyErr_Format(PyExc_TypeError, "expected byte string object for "
                         "status, value containing non 'latin-1' characters "
                         "found");
            return NULL;
        }

        item = latin_item;
    }
#endif

    if (!PyString_Check(item)) {
        PyErr_Format(PyExc_TypeError, "expected byte string object for "
                     "status, value of type %.200s found",
                     item->ob_type->tp_name);
        Py_XDECREF(latin_item);
        return NULL;
    }

    status = PyString_AsString(item);

    if (!PyList_Check(headers)) {
        PyErr_SetString(PyExc_TypeError, "response headers must be a list");
        Py_XDECREF(latin_item);
        return NULL;
    }

    if (exc_info && exc_info != Py_None) {
        if (self->status_line && !self->headers) {
            PyObject *type = NULL;
            PyObject *value = NULL;
            PyObject *traceback = NULL;

            if (!PyArg_ParseTuple(exc_info, "OOO", &type,
                                  &value, &traceback)) {
                Py_XDECREF(latin_item);
                return NULL;
            }

            Py_INCREF(type);
            Py_INCREF(value);
            Py_INCREF(traceback);

            PyErr_Restore(type, value, traceback);

            Py_XDECREF(latin_item);

            return NULL;
        }
    }
    else if (self->status_line && !self->headers) {
        PyErr_SetString(PyExc_RuntimeError, "headers have already been sent");
        Py_XDECREF(latin_item);
        return NULL;
    }

    self->status_line = apr_pstrdup(self->r->pool, status);

    value = ap_getword(self->r->pool, &status, ' ');

    errno = 0;
    self->status = strtol(value, &value, 10);

    if (*value || errno == ERANGE) {
        PyErr_SetString(PyExc_TypeError, "status value is not an integer");
        Py_XDECREF(latin_item);
        return NULL;
    }

    if (!*status) {
        PyErr_SetString(PyExc_ValueError, "status message was not supplied");
        Py_XDECREF(latin_item);
        return NULL;
    }

    Py_XDECREF(self->headers);

    self->headers = headers;

    Py_INCREF(self->headers);

    Py_XDECREF(latin_item);

    return PyObject_GetAttrString((PyObject *)self, "write");
}