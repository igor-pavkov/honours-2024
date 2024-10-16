static PyObject *Stream_iternext(StreamObject *self)
{
    PyObject *method = NULL;
    PyObject *args = NULL;
    PyObject *result = NULL;

    if (!self->adapter->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    method = PyObject_GetAttrString(self->filelike, "read");

    if (!method) {
        PyErr_SetString(PyExc_KeyError,
                        "file like object has no read() method");
        return 0;
    }

    args = Py_BuildValue("(l)", self->blksize);
    result = PyEval_CallObject(method, args);

    Py_DECREF(method);
    Py_DECREF(args);

    if (!result)
        return 0;

    if (PyString_Check(result)) {
        if (PyString_Size(result) == 0) {
            PyErr_SetObject(PyExc_StopIteration, Py_None);
            Py_DECREF(result);
            return 0;
        }

        return result;
    }

#if PY_MAJOR_VERSION >= 3
    if (PyUnicode_Check(result)) {
        if (PyUnicode_GetSize(result) == 0) {
            PyErr_SetObject(PyExc_StopIteration, Py_None);
            Py_DECREF(result);
            return 0;
        }

        return result;
    }
#endif

    Py_DECREF(result);

    PyErr_SetString(PyExc_TypeError,
                    "file like object yielded non string type");

    return 0;
}