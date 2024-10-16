static PyObject *Input_read(InputObject *self, PyObject *args)
{
    long size = -1;

    PyObject *result = NULL;
    char *buffer = NULL;
    apr_size_t length = 0;
    int init = 0;

    apr_size_t n;

    if (!self->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "|l:read", &size))
        return NULL;

#if defined(MOD_WSGI_WITH_DAEMONS)
    if (wsgi_inactivity_timeout) {
        apr_thread_mutex_lock(wsgi_shutdown_lock);
        wsgi_inactivity_shutdown_time = apr_time_now();
        wsgi_inactivity_shutdown_time += wsgi_inactivity_timeout;
        apr_thread_mutex_unlock(wsgi_shutdown_lock);
    }
#endif

    init = self->init;

    if (!self->init) {
        if (!ap_should_client_block(self->r))
            self->done = 1;

        self->init = 1;
    }

    /* No point continuing if no more data to be consumed. */

    if (self->done && self->length == 0)
        return PyString_FromString("");

    /*
     * If requested size is zero bytes, then still need to pass
     * this through to Apache input filters so that any
     * 100-continue response is triggered. Only do this if very
     * first attempt to read data. Note that this will cause an
     * assertion failure in HTTP_IN input filter when Apache
     * maintainer mode is enabled. It is arguable that the
     * assertion check, which prohibits a zero length read,
     * shouldn't exist, as why should a zero length read be not
     * allowed if input filter processing still works when it
     * does occur.
     */

    if (size == 0) {
        if (!init) {
            char dummy[1];

            Py_BEGIN_ALLOW_THREADS
            n = ap_get_client_block(self->r, dummy, 0);
            Py_END_ALLOW_THREADS

            if (n == -1) {
                PyErr_SetString(PyExc_IOError, "request data read error");
                return NULL;
            }
        }

        return PyString_FromString("");
    }

    /*
     * First deal with case where size has been specified. After
     * that deal with case where expected that all remaining
     * data is to be read in and returned as one string.
     */

    if (size > 0) {
        /* Allocate string of the exact size required. */

        result = PyString_FromStringAndSize(NULL, size);

        if (!result)
            return NULL;

        buffer = PyString_AS_STRING((PyStringObject *)result);

        /* Copy any residual data from use of readline(). */

        if (self->buffer && self->length) {
            if (size >= self->length) {
                length = self->length;
                memcpy(buffer, self->buffer + self->offset, length);
                self->offset = 0;
                self->length = 0;
            }
            else {
                length = size;
                memcpy(buffer, self->buffer + self->offset, length);
                self->offset += length;
                self->length -= length;
            }
        }

        /* If all data residual buffer consumed then free it. */

        if (!self->length) {
            free(self->buffer);
            self->buffer = NULL;
        }

        /* Read in remaining data required to achieve size. */

        if (length < size) {
            while (length != size) {
                Py_BEGIN_ALLOW_THREADS
                n = ap_get_client_block(self->r, buffer + length,
                                        size - length);
                Py_END_ALLOW_THREADS

                if (n == -1) {
                    PyErr_SetString(PyExc_IOError, "request data read error");
                    Py_DECREF(result);
                    return NULL;
                }
                else if (n == 0) {
                    /* Have exhausted all the available input data. */

                    self->done = 1;
                    break;
                }

                length += n;
            }

            /*
             * Resize the final string. If the size reduction is
             * by more than 25% of the string size, then Python
             * will allocate a new block of memory and copy the
             * data into it.
             */

            if (length != size) {
                if (_PyString_Resize(&result, length))
                    return NULL;
            }
        }
    }
    else {
        /*
         * Here we are going to try and read in all the
         * remaining data. First we have to allocate a suitably
         * large string, but we can't fully trust the amount
         * that the request structure says is remaining based on
         * the original content length though, as an input
         * filter can insert/remove data from the input stream
         * thereby invalidating the original content length.
         * What we do is allow for an extra 25% above what we
         * have already buffered and what the request structure
         * says is remaining. A value of 25% has been chosen so
         * as to match best how Python handles resizing of
         * strings. Note that even though we do this and allow
         * all available content, strictly speaking the WSGI
         * specification says we should only read up until content
         * length. This though is because the WSGI specification
         * is deficient in dealing with the concept of mutating
         * input filters. Since read() with no argument is also
         * not allowed by WSGI specification implement it in the
         * way which is most logical and ensure that input data
         * is not truncated.
         */

        size = self->length;

        if (!self->r->read_chunked && self->r->remaining > 0)
            size += self->r->remaining;

        size = size + (size >> 2);

        if (size < 256)
            size = self->r->read_chunked ? 8192 : 256;

        /* Allocate string of the estimated size. */

        result = PyString_FromStringAndSize(NULL, size);

        if (!result)
            return NULL;

        buffer = PyString_AS_STRING((PyStringObject *)result);

        /*
         * Copy any residual data from use of readline(). The
         * residual should always be less in size than the
         * string we have allocated to hold it, so can consume
         * all of it.
         */

        if (self->buffer && self->length) {
            length = self->length;
            memcpy(buffer, self->buffer + self->offset, length);
            self->offset = 0;
            self->length = 0;

            free(self->buffer);
            self->buffer = NULL;
        }

        /* Now make first attempt at reading remaining data. */

        Py_BEGIN_ALLOW_THREADS
        n = ap_get_client_block(self->r, buffer + length, size - length);
        Py_END_ALLOW_THREADS

        if (n == -1) {
            PyErr_SetString(PyExc_IOError, "request data read error");
            Py_DECREF(result);
            return NULL;
        }
        else if (n == 0) {
            /* Have exhausted all the available input data. */

            self->done = 1;
        }

        length += n;

        /*
         * Don't just assume that all data has been read if
         * amount read was less than that requested. Still must
         * perform a read which returns that no more data found.
         */

        while (!self->done) {
            if (length == size) {
                /* Increase the size of the string by 25%. */

                size = size + (size >> 2);

                if (_PyString_Resize(&result, size))
                    return NULL;

                buffer = PyString_AS_STRING((PyStringObject *)result);
            }

            /* Now make succesive attempt at reading data. */

            Py_BEGIN_ALLOW_THREADS
            n = ap_get_client_block(self->r, buffer + length, size - length);
            Py_END_ALLOW_THREADS

            if (n == -1) {
                PyErr_SetString(PyExc_IOError, "request data read error");
                Py_DECREF(result);
                return NULL;
            }
            else if (n == 0) {
                /* Have exhausted all the available input data. */

                self->done = 1;
            }

            length += n;
        }

        /*
         * Resize the final string. If the size reduction is by
         * more than 25% of the string size, then Python will
         * allocate a new block of memory and copy the data into
         * it.
         */

        if (length != size) {
            if (_PyString_Resize(&result, length))
                return NULL;
        }
    }

    return result;
}