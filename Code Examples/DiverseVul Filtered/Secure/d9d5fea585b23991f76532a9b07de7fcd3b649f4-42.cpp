static PyObject *Stream_close(StreamObject *self, PyObject *args)
{
    PyObject *method = NULL;
    PyObject *result = NULL;

    method = PyObject_GetAttrString(self->filelike, "close");

    if (method) {
        result = PyEval_CallObject(method, (PyObject *)NULL);
        if (!result)
            PyErr_Clear();
        Py_DECREF(method);
    }

    Py_XDECREF(result);

    Py_INCREF(Py_None);
    return Py_None;
}