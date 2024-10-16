static PyObject *Log_isatty(LogObject *self, PyObject *args)
{
    PyObject *result = NULL;

    if (!PyArg_ParseTuple(args, ":isatty"))
        return NULL;

    Py_INCREF(Py_False);
    return Py_False;
}