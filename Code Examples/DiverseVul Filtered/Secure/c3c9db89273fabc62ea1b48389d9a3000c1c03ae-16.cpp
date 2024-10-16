PyObject *PyString_AsEncodedObject(PyObject *str,
                                   const char *encoding,
                                   const char *errors)
{
    PyObject *v;

    if (!PyString_Check(str)) {
        PyErr_BadArgument();
        goto onError;
    }

    if (encoding == NULL) {
#ifdef Py_USING_UNICODE
        encoding = PyUnicode_GetDefaultEncoding();
#else
        PyErr_SetString(PyExc_ValueError, "no encoding specified");
        goto onError;
#endif
    }

    /* Encode via the codec registry */
    v = _PyCodec_EncodeText(str, encoding, errors);
    if (v == NULL)
        goto onError;

    return v;

 onError:
    return NULL;
}