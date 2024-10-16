static void Dispatch_dealloc(DispatchObject *self)
{
    Py_DECREF(self->log);

    PyObject_Del(self);
}