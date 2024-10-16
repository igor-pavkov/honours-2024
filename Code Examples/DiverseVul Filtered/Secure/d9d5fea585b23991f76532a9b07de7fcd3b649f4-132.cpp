static int Log_set_softspace(LogObject *self, PyObject *value)
{
    int new;

    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "can't delete softspace attribute");
        return -1;
    }

    new = PyInt_AsLong(value);
    if (new == -1 && PyErr_Occurred())
        return -1;

    self->softspace = new;

    return 0;
}