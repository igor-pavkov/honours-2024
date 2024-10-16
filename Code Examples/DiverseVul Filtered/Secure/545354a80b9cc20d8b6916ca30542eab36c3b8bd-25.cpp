static int Adapter_output(AdapterObject *self, const char *data,
                          apr_off_t length, PyObject *string_object,
                          int exception_when_aborted)
{
    int i = 0;
    apr_status_t rv;
    request_rec *r;

#if defined(MOD_WSGI_WITH_DAEMONS)
    if (wsgi_idle_timeout) {
        apr_thread_mutex_lock(wsgi_monitor_lock);

        if (wsgi_idle_timeout) {
            wsgi_idle_shutdown_time = apr_time_now();
            wsgi_idle_shutdown_time += wsgi_idle_timeout;
        }

        apr_thread_mutex_unlock(wsgi_monitor_lock);
    }
#endif

    if (!self->status_line) {
        PyErr_SetString(PyExc_RuntimeError, "response has not been started");
        return 0;
    }

    r = self->r;

    /* Have response headers yet been sent. */

    if (self->headers) {
        /*
         * Apache prior to Apache 2.2.8 has a bug in it
         * whereby it doesn't force '100 Continue'
         * response before responding with headers if no
         * read. So, force a zero length read before
         * sending the headers if haven't yet attempted
         * to read anything. This will ensure that if no
         * request content has been read that any '100
         * Continue' response will be flushed and sent
         * back to the client if client was expecting
         * one. Only want to do this for 2xx and 3xx
         * status values. Note that even though Apple
         * supplied version of Apache on MacOS X Leopard
         * is newer than version 2.2.8, the header file
         * has never been patched when they make updates
         * and so anything compiled against it thinks it
         * is older.
         */

#if (AP_SERVER_MAJORVERSION_NUMBER == 2 && \
     AP_SERVER_MINORVERSION_NUMBER < 2) || \
    (AP_SERVER_MAJORVERSION_NUMBER == 2 && \
     AP_SERVER_MINORVERSION_NUMBER == 2 && \
     AP_SERVER_PATCHLEVEL_NUMBER < 8)

        if (!self->input->init) {
            if (self->status >= 200 && self->status < 400) {
                PyObject *args = NULL;
                PyObject *result = NULL;
                args = Py_BuildValue("(i)", 0);
                result = Input_read(self->input, args);
                if (PyErr_Occurred())
                    PyErr_Clear();
                Py_DECREF(args);
                Py_XDECREF(result);
            }
        }

#endif

        /*
         * Now setup the response headers in request object. We
         * have already converted any native strings in the
         * headers to byte strings and validated the format of
         * the header names and values so can skip all the error
         * checking.
         */

        r->status = self->status;
        r->status_line = self->status_line;

        for (i = 0; i < PyList_Size(self->headers); i++) {
            PyObject *tuple = NULL;

            PyObject *object1 = NULL;
            PyObject *object2 = NULL;

            char *name = NULL;
            char *value = NULL;

            tuple = PyList_GetItem(self->headers, i);

            object1 = PyTuple_GetItem(tuple, 0);
            object2 = PyTuple_GetItem(tuple, 1);

            name = PyBytes_AsString(object1);
            value = PyBytes_AsString(object2);

            if (!strcasecmp(name, "Content-Type")) {
                /*
                 * In a daemon child process we cannot call the
                 * function ap_set_content_type() as want to
                 * avoid adding any output filters based on the
                 * type of file being served as this will be
                 * done in the main Apache child process which
                 * proxied the request to the daemon process.
                 */

                if (*self->config->process_group)
                    r->content_type = apr_pstrdup(r->pool, value);
                else
                    ap_set_content_type(r, apr_pstrdup(r->pool, value));
            }
            else if (!strcasecmp(name, "Content-Length")) {
                char *v = value;
                long l = 0;

                errno = 0;
                l = strtol(v, &v, 10);
                if (*v || errno == ERANGE || l < 0) {
                    PyErr_SetString(PyExc_ValueError,
                                    "invalid content length");
                    return 0;
                }

                ap_set_content_length(r, l);

                self->content_length_set = 1;
                self->content_length = l;
            }
            else if (!strcasecmp(name, "WWW-Authenticate")) {
                apr_table_add(r->err_headers_out, name, value);
            }
            else {
                apr_table_add(r->headers_out, name, value);
            }
        }

        /*
         * Reset flag indicating whether '100 Continue' response
         * expected. If we don't do this then if an attempt to read
         * input for the first time is after headers have been
         * sent, then Apache is wrongly generate the '100 Continue'
         * response into the response content. Not sure if this is
         * a bug in Apache, or that it truly believes that input
         * will never be read after the response headers have been
         * sent.
         */

        r->expecting_100 = 0;

        /* No longer need headers now that they have been sent. */

        Py_DECREF(self->headers);
        self->headers = NULL;
    }

    /*
     * If content length was specified, ensure that we don't
     * actually output more data than was specified as being
     * sent as otherwise technically in violation of HTTP RFC.
     */

    if (length) {
        apr_off_t output_length = length;

        if (self->content_length_set) {
            if (self->output_length < self->content_length) {
                if (self->output_length + length > self->content_length) {
                    length = self->content_length - self->output_length;
                }
            }
            else
                length = 0;
        }

        self->output_length += output_length;
    }

    /* Now output any data. */

    if (length) {
        apr_bucket *b;

        /*
         * When using Apache 2.X can use lower level
         * bucket brigade APIs. This is preferred as
         * ap_rwrite()/ap_rflush() will grow memory in
         * the request pool on each call, which will
         * result in an increase in memory use over time
         * when streaming of data is being performed.
         * The memory is still reclaimed, but only at
         * the end of the request. Using bucket brigade
         * API avoids this, and also avoids any copying
         * of response data due to buffering performed
         * by ap_rwrite().
         */

        if (r->connection->aborted) {
            if (!exception_when_aborted) {
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, self->r,
                              "mod_wsgi (pid=%d): Client closed connection.",
                              getpid());
            }
            else
                PyErr_SetString(PyExc_IOError, "client connection closed");

            return 0;
        }

        if (!self->bb) {
            self->bb = apr_brigade_create(r->pool,
                                          r->connection->bucket_alloc);
        }

#if 0
        if (string_object) {
            b = wsgi_apr_bucket_python_create(data, length,
                    self->config->application_group, string_object,
                    r->connection->bucket_alloc);
        }
        else {
#endif
            b = apr_bucket_transient_create(data, (apr_size_t)length,
                                            r->connection->bucket_alloc);
#if 0
        }
#endif

        APR_BRIGADE_INSERT_TAIL(self->bb, b);

        b = apr_bucket_flush_create(r->connection->bucket_alloc);
        APR_BRIGADE_INSERT_TAIL(self->bb, b);

        Py_BEGIN_ALLOW_THREADS
        rv = ap_pass_brigade(r->output_filters, self->bb);
        Py_END_ALLOW_THREADS

        if (rv != APR_SUCCESS) {
            PyErr_SetString(PyExc_IOError, "failed to write data");
            return 0;
        }

        Py_BEGIN_ALLOW_THREADS
        apr_brigade_cleanup(self->bb);
        Py_END_ALLOW_THREADS
    }

    /*
     * Check whether aborted connection was found when data
     * being written, otherwise will not be flagged until next
     * time that data is being written. Early detection is
     * better as it may have been the last data block being
     * written and application may think that data has all
     * been written. In a streaming application, we also want
     * to avoid any additional data processing to generate any
     * successive data.
     */

    if (r->connection->aborted) {
        if (!exception_when_aborted) {
            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, self->r,
                          "mod_wsgi (pid=%d): Client closed connection.",
                          getpid());
        }
        else
            PyErr_SetString(PyExc_IOError, "client connection closed");

        return 0;
    }

    return 1;
}