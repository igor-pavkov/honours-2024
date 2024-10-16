apr_status_t modsecurity_request_body_start(modsec_rec *msr, char **error_msg) {
    *error_msg = NULL;
    msr->msc_reqbody_length = 0;
    msr->stream_input_length = 0;

    /* Create a separate memory pool that will be used
     * to allocate structures from (not data, which is allocated
     * via malloc).
     */
    apr_pool_create(&msr->msc_reqbody_mp, NULL);

    /* Initialise request body processors, if any. */

    if (msr->msc_reqbody_processor != NULL) {
        char *my_error_msg = NULL;
        msre_reqbody_processor_metadata *metadata =
            (msre_reqbody_processor_metadata *)apr_table_get(msr->modsecurity->msre->reqbody_processors, msr->msc_reqbody_processor);

        if (metadata != NULL) {
            if (   (metadata->init != NULL)
                && (metadata->init(msr, &my_error_msg) < 0))
            {
                *error_msg = apr_psprintf(msr->mp,
                                          "%s parsing error (init): %s",
                                          msr->msc_reqbody_processor,
                                          my_error_msg);
                msr->msc_reqbody_error = 1;
                msr->msc_reqbody_error_msg = my_error_msg;
                msr_log(msr, 2, "%s", *error_msg);
            }
        }
        // TODO: All these below need to be registered in the same way as above
        else if (strcmp(msr->msc_reqbody_processor, "MULTIPART") == 0) {
            if (multipart_init(msr, &my_error_msg) < 0) {
                *error_msg = apr_psprintf(msr->mp, "Multipart parsing error (init): %s", my_error_msg);
                msr->msc_reqbody_error = 1;
                msr->msc_reqbody_error_msg = my_error_msg;
                msr_log(msr, 2, "%s", *error_msg);
            }
        }
        else if (strcmp(msr->msc_reqbody_processor, "XML") == 0) {
            if (xml_init(msr, &my_error_msg) < 0) {
                *error_msg = apr_psprintf(msr->mp, "XML parsing error (init): %s", my_error_msg);
                msr->msc_reqbody_error = 1;
                msr->msc_reqbody_error_msg = my_error_msg;
                msr_log(msr, 2, "%s", *error_msg);
            }
        }
        else if (strcmp(msr->msc_reqbody_processor, "URLENCODED") == 0) {
            /* Do nothing, URLENCODED processor does not support streaming yet. */
        }
        else {
            *error_msg = apr_psprintf(msr->mp, "Unknown request body processor: %s",
                msr->msc_reqbody_processor);
            return -1;
        }
    }

    return modsecurity_request_body_start_init(msr, error_msg);
}