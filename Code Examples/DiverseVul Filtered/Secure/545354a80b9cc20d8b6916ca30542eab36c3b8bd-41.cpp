static PyObject *Adapter_write(AdapterObject *self, PyObject *args)
{
    PyObject *item = NULL;
    PyObject *latin_item = NULL;
    const char *data = NULL;
    long length = 0;

    /* XXX The use of latin_item here looks very broken. */

    if (!self->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "O:write", &item))
        return NULL;

    if (!PyString_Check(item)) {
        PyErr_Format(PyExc_TypeError, "byte string value expected, value "
                     "of type %.200s found", item->ob_type->tp_name);
        Py_XDECREF(latin_item);
        return NULL;
    }

    data = PyString_AsString(item);
    length = PyString_Size(item);

    if (!Adapter_output(self, data, length, item, 1)) {
        Py_XDECREF(latin_item);
        return NULL;
    }

    Py_XDECREF(latin_item);

    Py_INCREF(Py_None);
    return Py_None;
}