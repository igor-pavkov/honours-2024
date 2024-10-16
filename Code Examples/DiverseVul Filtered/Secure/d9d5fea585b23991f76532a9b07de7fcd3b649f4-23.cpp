static PyObject *Adapter_ssl_is_https(AdapterObject *self, PyObject *args)
{
    APR_OPTIONAL_FN_TYPE(ssl_is_https) *ssl_is_https = 0;

    if (!self->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, ":ssl_is_https"))
        return NULL;

    ssl_is_https = APR_RETRIEVE_OPTIONAL_FN(ssl_is_https);

    if (ssl_is_https == 0)
      return Py_BuildValue("i", 0);

    return Py_BuildValue("i", ssl_is_https(self->r->connection));
}