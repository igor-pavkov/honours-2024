static void Adapter_dealloc(AdapterObject *self)
{
    Py_XDECREF(self->headers);
    Py_XDECREF(self->sequence);

    Py_DECREF(self->input);
    Py_DECREF(self->log);

    PyObject_Del(self);
}