static PyObject *Adapter_start_response(AdapterObject *self, PyObject *args)
{
    PyObject *result = NULL;

    PyObject *status_line = NULL;
    PyObject *headers = NULL;
    PyObject *exc_info = Py_None;

    PyObject *status_line_as_bytes = NULL;
    PyObject *headers_as_bytes = NULL;

    if (!self->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "OO!|O:start_response",
        &status_line, &PyList_Type, &headers, &exc_info)) {
        return NULL;
    }

    if (exc_info != Py_None && !PyTuple_Check(exc_info)) {
        PyErr_SetString(PyExc_RuntimeError, "exception info must be a tuple");
        return NULL;
    }

    if (exc_info != Py_None) {
        if (self->status_line && !self->headers) {
            PyObject *type = NULL;
            PyObject *value = NULL;
            PyObject *traceback = NULL;

            if (!PyArg_ParseTuple(exc_info, "OOO", &type,
                                  &value, &traceback)) {
                return NULL;
            }

            Py_INCREF(type);
            Py_INCREF(value);
            Py_INCREF(traceback);

            PyErr_Restore(type, value, traceback);

            return NULL;
        }
    }
    else if (self->status_line && !self->headers) {
        PyErr_SetString(PyExc_RuntimeError, "headers have already been sent");
        return NULL;
    }

    status_line_as_bytes = wsgi_convert_status_line_to_bytes(status_line);

    if (!status_line_as_bytes)
        goto finally;

    headers_as_bytes = wsgi_convert_headers_to_bytes(headers);

    if (!headers_as_bytes)
        goto finally;

    self->status_line = apr_pstrdup(self->r->pool, PyString_AsString(
                                    status_line_as_bytes));
    self->status = (int)strtol(self->status_line, NULL, 10);

    Py_XDECREF(self->headers);
    self->headers = headers_as_bytes;
    Py_INCREF(headers_as_bytes);

    result = PyObject_GetAttrString((PyObject *)self, "write");

finally:
    Py_XDECREF(status_line_as_bytes);
    Py_XDECREF(headers_as_bytes);

    return result;
}