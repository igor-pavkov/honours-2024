apr_status_t modsecurity_request_body_store(modsec_rec *msr,
    const char *data, apr_size_t length, char **error_msg)
{
    *error_msg = NULL;

    /* If we have a processor for this request body send
     * data to it first (but only if it did not report an
     * error on previous invocations).
     */
    if ((msr->msc_reqbody_processor != NULL) && (msr->msc_reqbody_error == 0)) {
        char *my_error_msg = NULL;
        msre_reqbody_processor_metadata *metadata =
            (msre_reqbody_processor_metadata *)apr_table_get(msr->modsecurity->msre->reqbody_processors, msr->msc_reqbody_processor);

        if (metadata != NULL) {
            if (   (metadata->process != NULL)
                && (metadata->process(msr, data, length, &my_error_msg) < 0))
            {
                *error_msg = apr_psprintf(msr->mp,
                                          "%s parsing error: %s",
                                          msr->msc_reqbody_processor,
                                          my_error_msg);
                msr->msc_reqbody_error = 1;
                msr->msc_reqbody_error_msg = my_error_msg;
                msr_log(msr, 2, "%s", *error_msg);
            }
        }
        // TODO: All these below need to be registered in the same way as above
        else if (strcmp(msr->msc_reqbody_processor, "MULTIPART") == 0) {
            /* The per-request data length counter will
             * be updated by the multipart parser.
             */

            /* Process data as multipart/form-data. */
            if (multipart_process_chunk(msr, data, length, &my_error_msg) < 0) {
                *error_msg = apr_psprintf(msr->mp, "Multipart parsing error: %s", my_error_msg);
                msr->msc_reqbody_error = 1;
                msr->msc_reqbody_error_msg = *error_msg;
                msr_log(msr, 2, "%s", *error_msg);
            }
        }
        else if (strcmp(msr->msc_reqbody_processor, "XML") == 0) {
            /* Increase per-request data length counter. */
            msr->msc_reqbody_no_files_length += length;

            /* Process data as XML. */
            if (xml_process_chunk(msr, data, length, &my_error_msg) < 0) {
                *error_msg = apr_psprintf(msr->mp, "XML parsing error: %s", my_error_msg);
                msr->msc_reqbody_error = 1;
                msr->msc_reqbody_error_msg = *error_msg;
                msr_log(msr, 2, "%s", *error_msg);
            }
        }
        else if (strcmp(msr->msc_reqbody_processor, "URLENCODED") == 0) {
            /* Increase per-request data length counter. */
            msr->msc_reqbody_no_files_length += length;

            /* Do nothing else, URLENCODED processor does not support streaming. */
        }
        else {
            *error_msg = apr_psprintf(msr->mp, "Unknown request body processor: %s",
                msr->msc_reqbody_processor);
            return -1;
        }
    } else if (msr->txcfg->reqbody_buffering != REQUEST_BODY_FORCEBUF_OFF) {
        /* Increase per-request data length counter if forcing buffering. */
        msr->msc_reqbody_no_files_length += length;
    }

    /* Check that we are not over the request body no files limit. */
    if (msr->msc_reqbody_no_files_length >= (unsigned long) msr->txcfg->reqbody_no_files_limit) {

        *error_msg = apr_psprintf(msr->mp, "Request body no files data length is larger than the "
                "configured limit (%ld).", msr->txcfg->reqbody_no_files_limit);
        if (msr->txcfg->debuglog_level >= 1) {
            msr_log(msr, 1, "%s", *error_msg);
        }

        if ((msr->txcfg->is_enabled == MODSEC_ENABLED) && (msr->txcfg->if_limit_action == REQUEST_BODY_LIMIT_ACTION_REJECT))   {
            return -5;
        } else if (msr->txcfg->if_limit_action == REQUEST_BODY_LIMIT_ACTION_PARTIAL)  {
            if(msr->txcfg->is_enabled == MODSEC_ENABLED)
                return -5;
        }
    }


    /* Store data. */
    if (msr->msc_reqbody_storage == MSC_REQBODY_MEMORY) {
        return modsecurity_request_body_store_memory(msr, data, length, error_msg);
    }
    else
        if (msr->msc_reqbody_storage == MSC_REQBODY_DISK) {
        return modsecurity_request_body_store_disk(msr, data, length, error_msg);
    }

    /* Should never happen. */
    *error_msg = apr_psprintf(msr->mp, "Internal error, unknown value for msc_reqbody_storage: %u",
        msr->msc_reqbody_storage);
    return -1;
}