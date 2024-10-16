static PyObject *newStreamObject(AdapterObject *adapter, PyObject *filelike,
                                 apr_size_t blksize)
{
    StreamObject *self;

    self = PyObject_New(StreamObject, &Stream_Type);
    if (self == NULL)
        return NULL;

    self->adapter = adapter;
    self->filelike = filelike;
    self->blksize = blksize;

    Py_INCREF(self->adapter);
    Py_INCREF(self->filelike);

    return (PyObject *)self;
}