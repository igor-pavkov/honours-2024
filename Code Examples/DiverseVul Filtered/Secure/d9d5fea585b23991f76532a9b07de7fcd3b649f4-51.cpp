static void Log_file(LogObject *self, const char *s, int l)
{
    /*
     * XXX This function is not currently being used.
     * The intention was that it be called instead of
     * Log_call() when 'target' is non zero. This would
     * be the case for 'stdout' and 'stderr'. Doing
     * this bypasses normally Apache logging mechanisms
     * though. May reawaken this code in mod_wsgi 4.0
     * by way of a mechanism to divert logging from a
     * daemon process to specfic log file or pipe using
     * an option to WSGIDaemonProcess.
     */

    char errstr[MAX_STRING_LEN];

    int plen = 0;
    int slen = 0;

#if AP_SERVER_MAJORVERSION_NUMBER < 2
    FILE *logf;
#else
    apr_file_t *logf = NULL;
#endif

    if (self->r)
        logf = self->r->server->error_log;
    else
        logf = wsgi_server->error_log;

#if AP_SERVER_MAJORVERSION_NUMBER < 2
    plen = ap_snprintf(errstr, sizeof(errstr), "[%s] ", ap_get_time());
#else
    errstr[0] = '[';
    ap_recent_ctime(errstr + 1, apr_time_now());
    errstr[1 + APR_CTIME_LEN - 1] = ']';
    errstr[1 + APR_CTIME_LEN    ] = ' ';
    plen = 1 + APR_CTIME_LEN + 1;
#endif

    if (self->target) {
        int len;

        errstr[plen++] = '[';

        len = strlen(self->target);
        memcpy(errstr+plen, self->target, len);

        plen += len;

        errstr[plen++] = ']';
        errstr[plen++] = ' ';
    }

    slen = MAX_STRING_LEN - plen - 1;

    Py_BEGIN_ALLOW_THREADS

    /*
     * We actually break long lines up into segments
     * of around 8192 characters, with the date/time
     * and target information prefixing each line.
     * This is just to avoid having to allocate more
     * memory just to format the line with prefix.
     * We want to avoid writing the prefix separately
     * so at least try and write line in one atomic
     * operation.
     */

    while (1) {
        if (l > slen) {
            memcpy(errstr+plen, s, slen);
            errstr[plen+slen] = '\n';
#if AP_SERVER_MAJORVERSION_NUMBER < 2
            fwrite(errstr, plen+slen+1, 1, logf);
            fflush(logf);
#else
            apr_file_write_full(logf, errstr, plen+slen+1, NULL);
            apr_file_flush(logf);
#endif
            s += slen;
            l -= slen;
        }
        else {
            memcpy(errstr+plen, s, l);
            errstr[plen+l] = '\n';
#if AP_SERVER_MAJORVERSION_NUMBER < 2
            fwrite(errstr, plen+l+1, 1, logf);
            fflush(logf);
#else
            apr_file_write_full(logf, errstr, plen+l+1, NULL);
            apr_file_flush(logf);
#endif
            break;
        }
    }

    Py_END_ALLOW_THREADS
}