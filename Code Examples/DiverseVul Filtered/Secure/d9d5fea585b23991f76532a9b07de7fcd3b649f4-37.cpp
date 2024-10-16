static PyObject *Log_writelines(LogObject *self, PyObject *args)
{
    PyObject *sequence = NULL;
    PyObject *iterator = NULL;
    PyObject *item = NULL;
    const char *msg = NULL;

    if (self->expired) {
        PyErr_SetString(PyExc_RuntimeError, "log object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "O:writelines", &sequence))
        return NULL;

    iterator = PyObject_GetIter(sequence);

    if (iterator == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "argument must be sequence of strings");

        return NULL;
    }

    while ((item = PyIter_Next(iterator))) {
        PyObject *result = NULL;
        PyObject *args = NULL;

        args = PyTuple_Pack(1, item);

        result = Log_write(self, args);

        Py_DECREF(args);
        Py_DECREF(item);

        if (!result) {
            Py_DECREF(iterator);

            PyErr_SetString(PyExc_TypeError,
                            "argument must be sequence of strings");

            return NULL;
        }
    }

    Py_DECREF(iterator);

    Py_INCREF(Py_None);
    return Py_None;
}