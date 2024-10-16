static AdapterObject *newAdapterObject(request_rec *r)
{
    AdapterObject *self;

    self = PyObject_New(AdapterObject, &Adapter_Type);
    if (self == NULL)
        return NULL;

    self->result = HTTP_INTERNAL_SERVER_ERROR;

    self->r = r;

#if defined(MOD_WSGI_WITH_BUCKETS)
    self->bb = NULL;
#endif

    self->config = (WSGIRequestConfig *)ap_get_module_config(r->request_config,
                                                             &wsgi_module);

    self->status = HTTP_INTERNAL_SERVER_ERROR;
    self->status_line = NULL;
    self->headers = NULL;
    self->sequence = NULL;

    self->content_length_set = 0;
    self->content_length = 0;
    self->output_length = 0;

    self->input = newInputObject(r);
    self->log = newLogObject(r, APLOG_ERR, NULL);

    return self;
}