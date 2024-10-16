static PyObject *Log_closed(LogObject *self, void *closure)
{
    Py_INCREF(Py_False);
    return Py_False;
}