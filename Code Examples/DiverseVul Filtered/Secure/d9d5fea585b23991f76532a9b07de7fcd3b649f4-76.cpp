static PyObject *Stream_iter(StreamObject *self)
{
    if (!self->adapter->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    Py_INCREF(self);
    return (PyObject *)self;
}