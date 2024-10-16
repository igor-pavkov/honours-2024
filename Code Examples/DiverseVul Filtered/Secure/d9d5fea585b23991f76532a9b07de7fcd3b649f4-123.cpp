static PyObject *Log_get_encoding(LogObject *self, void *closure)
{
    return PyUnicode_FromString("utf-8");
}