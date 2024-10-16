static PyObject *Log_get_softspace(LogObject *self, void *closure)
{
    return PyInt_FromLong(self->softspace);
}