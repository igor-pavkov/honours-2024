static void Log_call(LogObject *self, const char *s, int l)
{
    /*
     * The length of the string to be logged is ignored
     * for now. We just pass the whole string to the
     * Apache error log functions. It will actually
     * truncate it at some value less than 8192
     * characters depending on the length of the prefix
     * to go at the front. If there are embedded NULLs
     * then truncation will occur at that point. That
     * truncation occurs like this is also what happens
     * if using FASTCGI solutions for Apache, so not
     * doing anything different here.
     */

    if (self->r) {
        Py_BEGIN_ALLOW_THREADS
        ap_log_rerror(APLOG_MARK, WSGI_LOG_LEVEL(self->level),
                      self->r, "%s", s);
        Py_END_ALLOW_THREADS
    }
    else {
        Py_BEGIN_ALLOW_THREADS
        ap_log_error(APLOG_MARK, WSGI_LOG_LEVEL(self->level),
                     wsgi_server, "%s", s);
        Py_END_ALLOW_THREADS
    }
}