static PyObject *Log_seekable(LogObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":seekable"))
        return NULL;

    Py_INCREF(Py_False);
    return Py_False;
}