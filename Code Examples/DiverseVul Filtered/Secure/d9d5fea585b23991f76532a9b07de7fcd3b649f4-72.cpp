static PyObject *Input_readlines(InputObject *self, PyObject *args)
{
    long hint = 0;
    long length = 0;

    PyObject *result = NULL;
    PyObject *line = NULL;
    PyObject *rlargs = NULL;

    if (!self->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "|l:readlines", &hint))
        return NULL;

    result = PyList_New(0);
    if (!result)
        return NULL;

    rlargs = PyTuple_New(0);
    if (!rlargs) {
        Py_DECREF(result);
        return NULL;
    }

    while (1) {
        int n;

        if (!(line = Input_readline(self, rlargs))) {
            Py_DECREF(result);
            result = NULL;
            break;
        }

        if ((n = PyString_Size(line)) == 0) {
            Py_DECREF(line);
            break;
        }

        if (PyList_Append(result, line) == -1) {
            Py_DECREF(line);
            Py_DECREF(result);
            result = NULL;
            break;
        }

        Py_DECREF(line);

        length += n;
        if (hint > 0 && length >= hint)
            break;
    }

    Py_DECREF(rlargs);

    return result;
}