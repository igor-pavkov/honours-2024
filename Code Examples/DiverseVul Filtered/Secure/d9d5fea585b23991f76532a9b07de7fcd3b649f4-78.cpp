static void Restricted_dealloc(RestrictedObject *self)
{
    PyObject_Del(self);
}