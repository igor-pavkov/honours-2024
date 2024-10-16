static PyObject *Adapter_write(AdapterObject *self, PyObject *args)
{
    PyObject *item = NULL;
    PyObject *latin_item = NULL;
    const char *data = NULL;
    int length = 0;

    if (!self->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "O:write", &item))
        return NULL;

#if PY_MAJOR_VERSION >= 3
    if (PyUnicode_Check(item)) {
        latin_item = PyUnicode_AsLatin1String(item);
        if (!latin_item) {
            PyErr_Format(PyExc_TypeError, "byte string value expected, "
                         "value containing non 'latin-1' characters found");
            return NULL;
        }

        Py_DECREF(item);
        item = latin_item;
    }
#endif

    if (!PyString_Check(item)) {
        PyErr_Format(PyExc_TypeError, "byte string value expected, value "
                     "of type %.200s found", item->ob_type->tp_name);
        Py_XDECREF(latin_item);
        return NULL;
    }

    data = PyString_AsString(item);
    length = PyString_Size(item);

    if (!Adapter_output(self, data, length, 1)) {
        Py_XDECREF(latin_item);
        return NULL;
    }

    Py_XDECREF(latin_item);

    Py_INCREF(Py_None);
    return Py_None;
}