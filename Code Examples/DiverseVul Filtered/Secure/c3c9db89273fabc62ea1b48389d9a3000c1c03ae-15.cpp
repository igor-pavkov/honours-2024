PyObject *PyString_Encode(const char *s,
                          Py_ssize_t size,
                          const char *encoding,
                          const char *errors)
{
    PyObject *v, *str;

    str = PyString_FromStringAndSize(s, size);
    if (str == NULL)
        return NULL;
    v = PyString_AsEncodedString(str, encoding, errors);
    Py_DECREF(str);
    return v;
}