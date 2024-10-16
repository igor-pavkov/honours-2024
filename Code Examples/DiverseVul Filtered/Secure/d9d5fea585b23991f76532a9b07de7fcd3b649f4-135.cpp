static PyObject *Log_write(LogObject *self, PyObject *args)
{
    const char *msg = NULL;
    int len = -1;

    if (self->expired) {
        PyErr_SetString(PyExc_RuntimeError, "log object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "s#:write", &msg, &len))
        return NULL;

    Log_queue(self, msg, len);

    Py_INCREF(Py_None);
    return Py_None;
}