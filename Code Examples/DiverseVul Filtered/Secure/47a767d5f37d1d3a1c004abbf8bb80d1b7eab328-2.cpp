char *am_getfile(apr_pool_t *conf, server_rec *s, const char *file)
{
    apr_status_t rv;
    char buffer[512];
    apr_finfo_t finfo;
    char *data;
    apr_file_t *fd;
    apr_size_t nbytes;

    if (file == NULL)
        return NULL;

    if ((rv = apr_file_open(&fd, file, APR_READ, 0, conf)) != 0) {
        ap_log_error(APLOG_MARK, APLOG_ERR, rv, s,
                     "apr_file_open: Error opening \"%s\" [%d] \"%s\"",
                     file, rv, apr_strerror(rv, buffer, sizeof(buffer)));
        return NULL;
    }

    if ((rv = apr_file_info_get(&finfo, APR_FINFO_SIZE, fd)) != 0) {
        ap_log_error(APLOG_MARK, APLOG_ERR, rv, s,
                     "apr_file_info_get: Error opening \"%s\" [%d] \"%s\"",
                     file, rv, apr_strerror(rv, buffer, sizeof(buffer)));
        (void)apr_file_close(fd);
        return NULL;
    }

    nbytes = finfo.size;
    data = (char *)apr_palloc(conf, nbytes + 1);

    rv = apr_file_read_full(fd, data, nbytes, NULL);
    if (rv != 0) {
        ap_log_error(APLOG_MARK, APLOG_ERR, rv, s,
                     "apr_file_read_full: Error reading \"%s\" [%d] \"%s\"",
                     file, rv, apr_strerror(rv, buffer, sizeof(buffer)));
    }
    data[nbytes] = '\0';

    (void)apr_file_close(fd);

    return data;
}