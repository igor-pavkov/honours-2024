static PyObject *newLogObject(request_rec *r, int level, const char *target)
{
    LogObject *self;

#if PY_MAJOR_VERSION >= 3
    PyObject *module = NULL;
    PyObject *dict = NULL;
    PyObject *object = NULL;
    PyObject *args = NULL;
    PyObject *result = NULL;

    module = PyImport_ImportModule("io");

    if (!module)
        return NULL;

    dict = PyModule_GetDict(module);
    object = PyDict_GetItemString(dict, "TextIOWrapper");

    if (!object) {
        PyErr_SetString(PyExc_NameError,
                        "name 'TextIOWrapper' is not defined");
        return NULL;
    }
#endif

    self = PyObject_New(LogObject, &Log_Type);
    if (self == NULL)
        return NULL;

    self->target = target;
    self->r = r;
    self->level = APLOG_NOERRNO|level;
    self->s = NULL;
    self->l = 0;
    self->expired = 0;
#if PY_MAJOR_VERSION < 3
    self->softspace = 0;
#endif

#if PY_MAJOR_VERSION >= 3
    Py_INCREF(object);
    args = Py_BuildValue("(OssOO)", self, "utf-8", "replace",
                         Py_None, Py_True);
    Py_DECREF(self);
    result = PyEval_CallObject(object, args);
    Py_DECREF(args);
    Py_DECREF(object);

    return result;
#else
    return (PyObject *)self;
#endif
}