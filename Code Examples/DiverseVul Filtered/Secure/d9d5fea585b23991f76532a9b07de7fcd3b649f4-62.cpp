static PyObject *Adapter_file_wrapper(AdapterObject *self, PyObject *args)
{
    PyObject *filelike = NULL;
    apr_size_t blksize = HUGE_STRING_LEN;
    PyObject *result = NULL;

    if (!self->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "O|l:file_wrapper", &filelike, &blksize))
        return NULL;

    return newStreamObject(self, filelike, blksize);
}