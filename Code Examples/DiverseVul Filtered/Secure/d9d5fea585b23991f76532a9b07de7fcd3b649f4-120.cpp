static int Adapter_process_file_wrapper(AdapterObject *self)
{
    int done = 0;

#ifndef WIN32
#if AP_SERVER_MAJORVERSION_NUMBER >= 2

    PyObject *filelike = NULL;
    PyObject *method = NULL;
    PyObject *object = NULL;

    apr_status_t rv = 0;

    apr_os_file_t fd = -1;
    apr_file_t *tmpfile = NULL;
    apr_finfo_t finfo;

    apr_off_t fd_offset = 0;
    apr_off_t fo_offset = 0;

    apr_off_t length = 0;

    /* Perform file wrapper optimisations where possible. */

    if (self->sequence->ob_type != &Stream_Type)
        return 0;

    /*
     * Only attempt to perform optimisations if the
     * write() function returned by start_response()
     * function has not been called with non zero length
     * data. In other words if no prior response content
     * generated. Technically it could be done, but want
     * to have a consistent rule about how specifying a
     * content length affects how much of a file is
     * sent. Don't want to have to take into
     * consideration whether write() function has been
     * called or not as just complicates things.
     */

    if (self->output_length != 0)
        return 0;

    /*
     * Work out if file wrapper is associated with a
     * file like object, where that file object is
     * associated with a regular file. If it does then
     * we can optimise how the contents of the file are
     * sent out. If no such associated file descriptor
     * then it needs to be processed like any other
     * iterable value.
     */

    filelike = ((StreamObject *)self->sequence)->filelike;

    fd = PyObject_AsFileDescriptor(filelike);
    if (fd == -1) {
        PyErr_Clear();
        return 0;
    }

    /*
     * On some platforms, such as Linux, sendfile() system call
     * will not work on UNIX sockets. Thus when using daemon mode
     * cannot enable that feature.
     */

    if (self->config->enable_sendfile)
        apr_os_file_put(&tmpfile, &fd, APR_SENDFILE_ENABLED, self->r->pool);
    else
        apr_os_file_put(&tmpfile, &fd, 0, self->r->pool);

    rv = apr_file_info_get(&finfo, APR_FINFO_SIZE|APR_FINFO_TYPE, tmpfile);
    if (rv != APR_SUCCESS || finfo.filetype != APR_REG)
        return 0;

    /*
     * Because Python file like objects potentially have
     * their own buffering layering, or use an operating
     * system FILE object which also has a buffering
     * layer on top of a normal file descriptor, need to
     * determine from the file like object its position
     * within the file and use that as starting position.
     * Note that it is assumed that user had flushed any
     * modifications to the file as necessary. Also, we
     * need to make sure we remember the original file
     * descriptor position as will need to restore that
     * position so it matches the upper buffering layers
     * when done. This is done to avoid any potential
     * problems if file like object does anything strange
     * in its close() method which relies on file position
     * being what it thought it should be.
     */

    rv = apr_file_seek(tmpfile, APR_CUR, &fd_offset);
    if (rv != APR_SUCCESS)
        return 0;

    method = PyObject_GetAttrString(filelike, "tell");
    if (!method)
        return 0;

    object = PyEval_CallObject(method, NULL);
    Py_DECREF(method);

    if (!object) {
        PyErr_Clear();
        return 0;
    }

   if (PyLong_Check(object)) {
#if defined(HAVE_LONG_LONG)
       fo_offset = PyLong_AsLongLong(object);
#else
       fo_offset = PyLong_AsLong(object);
#endif
   }
#if PY_MAJOR_VERSION < 3
   else if (PyInt_Check(object)) {
       fo_offset = PyInt_AsLong(object);
   }
#endif
   else {
       Py_DECREF(object);
       return 0;
   }

   if (PyErr_Occurred()){
       Py_DECREF(object);
       PyErr_Clear();
       return 0;
   }

    Py_DECREF(object);

    /*
     * For a file wrapper object need to always ensure
     * that response headers are parsed. This is done so
     * that if the content length header has been
     * defined we can get its value and use it to limit
     * how much of a file is being sent. The WSGI 1.0
     * specification says that we are meant to send all
     * available bytes from the file, however this is
     * questionable as sending more than content length
     * would violate HTTP RFC. Note that this doesn't
     * actually flush the headers out when using Apache
     * 2.X. This is good, as we want to still be able to
     * set the content length header if none set and file
     * is seekable. If processing response headers fails,
     * then need to return as if done, with error being
     * logged later.
     */

    if (!Adapter_output(self, "", 0, 0))
        return 1;

    /*
     * If content length wasn't defined then determine
     * the amount of data which is available to send and
     * set the content length response header. Either
     * way, if can work out length then send data
     * otherwise fall through and treat it as normal
     * iterable.
     */

    if (!self->content_length_set) {
        length = finfo.size - fo_offset;
        self->output_length += length;

        ap_set_content_length(self->r, length);

        self->content_length_set = 1;
        self->content_length = length;

        if (Adapter_output_file(self, tmpfile, fo_offset, length))
            self->result = OK;

        done = 1;
    }
    else {
        length = finfo.size - fo_offset;
        self->output_length += length;

        /* Use user specified content length instead. */

        length = self->content_length;

        if (Adapter_output_file(self, tmpfile, fo_offset, length))
            self->result = OK;

        done = 1;
    }

    /*
     * Restore position of underlying file descriptor.
     * If this fails, then not much we can do about it.
     */

    apr_file_seek(tmpfile, APR_SET, &fd_offset);

#endif
#endif

    return done;
}