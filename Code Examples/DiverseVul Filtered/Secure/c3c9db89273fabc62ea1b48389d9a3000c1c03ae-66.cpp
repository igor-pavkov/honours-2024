PyObject *PyString_AsDecodedString(PyObject *str,
                                   const char *encoding,
                                   const char *errors)
{
    PyObject *v;

    v = PyString_AsDecodedObject(str, encoding, errors);
    if (v == NULL)
        goto onError;

#ifdef Py_USING_UNICODE
    /* Convert Unicode to a string using the default encoding */
    if (PyUnicode_Check(v)) {
        PyObject *temp = v;
        v = PyUnicode_AsEncodedString(v, NULL, NULL);
        Py_DECREF(temp);
        if (v == NULL)
            goto onError;
    }
#endif
    if (!PyString_Check(v)) {
        PyErr_Format(PyExc_TypeError,
                     "decoder did not return a string object (type=%.400s)",
                     Py_TYPE(v)->tp_name);
        Py_DECREF(v);
        goto onError;
    }

    return v;

 onError:
    return NULL;
}