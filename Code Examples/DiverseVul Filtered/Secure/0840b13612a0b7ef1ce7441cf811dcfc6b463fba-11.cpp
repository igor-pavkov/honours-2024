apr_status_t modsecurity_request_body_end(modsec_rec *msr, char **error_msg) {
    *error_msg = NULL;

    /* Close open file descriptors, if any. */
    if (msr->msc_reqbody_storage == MSC_REQBODY_DISK) {
        if (msr->msc_reqbody_fd > 0) {
            close(msr->msc_reqbody_fd);
            msr->msc_reqbody_fd = -1;
        }
    }

    /* Note that we've read the body. */
    msr->msc_reqbody_read = 1;

    /* Finalise body processing. */
    if ((msr->msc_reqbody_processor != NULL) && (msr->msc_reqbody_error == 0)) {
        char *my_error_msg = NULL;
        msre_reqbody_processor_metadata *metadata =
            (msre_reqbody_processor_metadata *)apr_table_get(msr->modsecurity->msre->reqbody_processors, msr->msc_reqbody_processor);

        if (metadata != NULL) {
            if (   (metadata->complete != NULL)
                && (metadata->complete(msr, &my_error_msg) < 0))
            {
                *error_msg = apr_psprintf(msr->mp,
                                          "%s parsing error (complete): %s",
                                          msr->msc_reqbody_processor,
                                          my_error_msg);
                msr->msc_reqbody_error = 1;
                msr->msc_reqbody_error_msg = my_error_msg;
                msr_log(msr, 2, "%s", *error_msg);
            }
        }
        // TODO: All these below need to be registered in the same way as above
        else if (strcmp(msr->msc_reqbody_processor, "MULTIPART") == 0) {
            if (multipart_complete(msr, &my_error_msg) < 0) {
                *error_msg = apr_psprintf(msr->mp, "Multipart parsing error: %s", my_error_msg);
                msr->msc_reqbody_error = 1;
                msr->msc_reqbody_error_msg = *error_msg;
                if (msr->txcfg->debuglog_level >= 4) {
                    msr_log(msr, 4, "%s", *error_msg);
                }
                return -1;
            }

            if (multipart_get_arguments(msr, "BODY", msr->arguments) < 0) {
                *error_msg = "Multipart parsing error: Failed to retrieve arguments.";
                msr->msc_reqbody_error = 1;
                msr->msc_reqbody_error_msg = *error_msg;
                msr_log(msr, 2, "%s", *error_msg);
                return -1;
            }
        }
        else if (strcmp(msr->msc_reqbody_processor, "URLENCODED") == 0) {
            return modsecurity_request_body_end_urlencoded(msr, error_msg);
        }
        else if (strcmp(msr->msc_reqbody_processor, "XML") == 0) {
            if (xml_complete(msr, &my_error_msg) < 0) {
                *error_msg = apr_psprintf(msr->mp, "XML parser error: %s", my_error_msg);
                msr->msc_reqbody_error = 1;
                msr->msc_reqbody_error_msg = *error_msg;
                msr_log(msr, 2, "%s", *error_msg);
                return -1;
            }
        }
    } else if (msr->txcfg->reqbody_buffering != REQUEST_BODY_FORCEBUF_OFF) {
        /* Convert to a single continous buffer, but don't do anything else. */
        return modsecurity_request_body_end_raw(msr, error_msg);
    }

    /* Note the request body no files length. */
    msr_log(msr, 4, "Request body no files length: %" APR_SIZE_T_FMT, msr->msc_reqbody_no_files_length);

    return 1;
}