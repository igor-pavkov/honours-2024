apr_status_t modsecurity_request_body_clear(modsec_rec *msr, char **error_msg) {
    *error_msg = NULL;

    /* Release memory we used to store request body data. */
    if (msr->msc_reqbody_chunks != NULL) {
        msc_data_chunk **chunks = (msc_data_chunk **)msr->msc_reqbody_chunks->elts;
        int i;

        for(i = 0; i < msr->msc_reqbody_chunks->nelts; i++) {
            if (chunks[i]->data != NULL) {
                free(chunks[i]->data);
                chunks[i]->data = NULL;
            }
        }
    }

    if (msr->msc_reqbody_storage == MSC_REQBODY_DISK) {
        int keep_body = 0;

        /* Should we keep the body? This normally
         * happens when a PUT method was used, which
         * means the body is actually a file.
         */
        if ((msr->upload_remove_files == 0)&&(strcasecmp(msr->request_method, "PUT") == 0)) {
            if (msr->txcfg->upload_dir != NULL) {
                keep_body = 1;
            } else {
                *error_msg = apr_psprintf(msr->mp, "Input filter: SecUploadDir is undefined, "
                    "unable to store PUT file.");
            }
        }

        /* Deal with a request body stored in a file. */

        if (msr->msc_reqbody_filename != NULL) {
            if (keep_body) {
                /* Move request body (which is a file) to the storage area. */
                const char *put_filename = NULL;
                const char *put_basename = NULL;

                /* Construct the new filename. */
                put_basename = file_basename(msr->msc_reqbody_mp, msr->msc_reqbody_filename);
                if (put_basename == NULL) {
                    *error_msg = apr_psprintf(msr->mp, "Input filter: Failed to generate basename to PUT file \"%s\"", log_escape(msr->msc_reqbody_mp, msr->msc_reqbody_filename));
                    return -1;
                }
                put_filename = apr_psprintf(msr->msc_reqbody_mp, "%s/%s",
                    msr->txcfg->upload_dir, put_basename);
                if (put_filename == NULL) {
                    *error_msg = apr_psprintf(msr->mp, "Input filter: Failed to generate filename to PUT file \"%s\"", log_escape(msr->msc_reqbody_mp, msr->msc_reqbody_filename));
                    return -1;
                }

                if (apr_file_rename(msr->msc_reqbody_filename, put_filename,
                    msr->msc_reqbody_mp) != APR_SUCCESS)
                {
                    *error_msg = apr_psprintf(msr->mp, "Input filter: Failed to rename file from \"%s\" to \"%s\".",
                        log_escape(msr->msc_reqbody_mp, msr->msc_reqbody_filename),
                        log_escape(msr->msc_reqbody_mp, put_filename));
                    return -1;
                } else {
                    msr_log(msr, 4, "Input filter: Moved file from \"%s\" to \"%s\".",
                        log_escape(msr->msc_reqbody_mp, msr->msc_reqbody_filename),
                        log_escape(msr->msc_reqbody_mp, put_filename));
                }
            } else {
                /* make sure it is closed first */
                if (msr->msc_reqbody_fd > 0) {
                    close(msr->msc_reqbody_fd);
                    msr->msc_reqbody_fd = -1;
                }

                /* We do not want to keep the request body. */
                if (apr_file_remove(msr->msc_reqbody_filename,
                    msr->msc_reqbody_mp) != APR_SUCCESS)
                {
                    *error_msg = apr_psprintf(msr->mp, "Input filter: Failed to delete temporary file: %s",
                        log_escape(msr->mp, msr->msc_reqbody_filename));
                    return -1;
                }

                msr_log(msr, 4, "Input filter: Removed temporary file: %s",
                    msr->msc_reqbody_filename);
            }

            msr->msc_reqbody_filename = NULL;
        }
    }

    if (msr->msc_reqbody_mp != NULL) {
        apr_pool_destroy(msr->msc_reqbody_mp);
        msr->msc_reqbody_mp = NULL;
    }

    return 1;
}