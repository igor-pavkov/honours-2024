static PyObject *Restricted_getattr(RestrictedObject *self, char *name)
{
    PyErr_Format(PyExc_IOError, "%s access restricted by mod_wsgi", self->s);

    return NULL;
}