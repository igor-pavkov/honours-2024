static apr_status_t modsecurity_request_body_end_urlencoded(modsec_rec *msr, char **error_msg) {
    int invalid_count = 0;

    *error_msg = NULL;

    /* Create the raw buffer */
    if (modsecurity_request_body_end_raw(msr, error_msg) != 1) {
        return -1;
    }

    /* Parse URL-encoded arguments in the request body. */

    if (parse_arguments(msr, msr->msc_reqbody_buffer, msr->msc_reqbody_length,
        msr->txcfg->argument_separator, "BODY", msr->arguments, &invalid_count) < 0)
    {
        *error_msg = apr_pstrdup(msr->mp, "Initialisation: Error occurred while parsing BODY arguments.");
        return -1;
    }

    if (invalid_count) {
        msr->urlencoded_error = 1;
    }

    return 1;
}