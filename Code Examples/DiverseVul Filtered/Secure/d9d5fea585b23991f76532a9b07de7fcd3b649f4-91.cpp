static PyObject *Log_close(LogObject *self, PyObject *args)
{
    PyObject *result = NULL;

    if (!PyArg_ParseTuple(args, ":close"))
        return NULL;

    if (!self->expired)
        result = Log_flush(self, args);

    Py_XDECREF(result);

    self->r = NULL;
    self->expired = 1;

    Py_INCREF(Py_None);
    return Py_None;
}