static int Adapter_output_file(AdapterObject *self, apr_file_t* tmpfile,
                               apr_off_t offset, apr_off_t len)
{
    request_rec *r;
    apr_bucket *b;
    apr_status_t rv;
    apr_bucket_brigade *bb;

    r = self->r;

    if (r->connection->aborted) {
        PyErr_SetString(PyExc_IOError, "client connection closed");
        return 0;
    }

    if (len == 0)
        return 1;

    bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);

    if (sizeof(apr_off_t) == sizeof(apr_size_t) || len < MAX_BUCKET_SIZE) {
        /* Can use a single bucket to send file. */

        b = apr_bucket_file_create(tmpfile, offset, (apr_size_t)len, r->pool,
                                   r->connection->bucket_alloc);
    }
    else {
        /* Need to create multiple buckets to send file. */

        b = apr_bucket_file_create(tmpfile, offset, MAX_BUCKET_SIZE, r->pool,
                                   r->connection->bucket_alloc);

        while (len > MAX_BUCKET_SIZE) {
            apr_bucket *cb;
            apr_bucket_copy(b, &cb);
            APR_BRIGADE_INSERT_TAIL(bb, cb);
            b->start += MAX_BUCKET_SIZE;
            len -= MAX_BUCKET_SIZE;
        }

        /* Resize just the last bucket */

        b->length = (apr_size_t)len;
    }

    APR_BRIGADE_INSERT_TAIL(bb, b);

    b = apr_bucket_flush_create(r->connection->bucket_alloc);
    APR_BRIGADE_INSERT_TAIL(bb, b);

    b = apr_bucket_eos_create(r->connection->bucket_alloc);
    APR_BRIGADE_INSERT_TAIL(bb, b);

    Py_BEGIN_ALLOW_THREADS
    rv = ap_pass_brigade(r->output_filters, bb);
    Py_END_ALLOW_THREADS

    if (rv != APR_SUCCESS) {
        PyErr_SetString(PyExc_IOError, "failed to write data");
        return 0;
    }

    Py_BEGIN_ALLOW_THREADS
    apr_brigade_destroy(bb);
    Py_END_ALLOW_THREADS

    if (r->connection->aborted) {
        PyErr_SetString(PyExc_IOError, "client connection closed");
        return 0;
    }

    return 1;
}