static apr_status_t lua_write_body(request_rec *r, apr_file_t *file, apr_off_t *size)
{
    apr_status_t rc = OK;

    if ((rc = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR)))
        return rc;
    if (ap_should_client_block(r)) {
        char argsbuffer[HUGE_STRING_LEN];
        apr_off_t rsize,
                  len_read,
                  rpos = 0;
        apr_off_t length = r->remaining;

        *size = length;
        while ((len_read =
                    ap_get_client_block(r, argsbuffer,
                                        sizeof(argsbuffer))) > 0) {
            if ((rpos + len_read) > length)
                rsize = (apr_size_t) length - rpos;
            else
                rsize = len_read;

            rc = apr_file_write_full(file, argsbuffer, (apr_size_t) rsize,
                                     NULL);
            if (rc != APR_SUCCESS)
                return rc;
            rpos += rsize;
        }
    }

    return rc;
}