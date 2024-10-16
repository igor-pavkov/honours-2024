static void Stream_dealloc(StreamObject *self)
{
    Py_DECREF(self->filelike);
    Py_DECREF(self->adapter);

    PyObject_Del(self);
}