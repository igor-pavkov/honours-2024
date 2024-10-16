static PyObject *Log_readable(LogObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":readable"))
        return NULL;

    Py_INCREF(Py_False);
    return Py_False;
}