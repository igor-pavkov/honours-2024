static DispatchObject *newDispatchObject(request_rec *r,
                                         WSGIRequestConfig *config)
{
    DispatchObject *self;

    self = PyObject_New(DispatchObject, &Dispatch_Type);
    if (self == NULL)
        return NULL;

    self->config = config;

    self->r = r;

    self->log = newLogObject(r, APLOG_ERR, NULL);

    return self;
}