static PyObject *Log_writable(LogObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":writable"))
        return NULL;

    Py_INCREF(Py_True);
    return Py_True;
}