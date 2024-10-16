static PyObject *Log_flush(LogObject *self, PyObject *args)
{
    if (self->expired) {
        PyErr_SetString(PyExc_RuntimeError, "log object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, ":flush"))
        return NULL;

    if (self->s) {
        Log_call(self, self->s, self->l);

        free(self->s);
        self->s = NULL;
        self->l = 0;
    }

    Py_INCREF(Py_None);
    return Py_None;
}