static apr_status_t modsecurity_request_body_store_disk(modsec_rec *msr,
    const char *data, apr_size_t length, char **error_msg)
{
    apr_size_t i;

    *error_msg = NULL;

    i = write(msr->msc_reqbody_fd, data, length);
    if (i != length) {
        *error_msg = apr_psprintf(msr->mp, "Input filter: Failed writing %" APR_SIZE_T_FMT
            " bytes to temporary file (rc %" APR_SIZE_T_FMT ").", length, i);
        return -1;
    }

    return 1;
}