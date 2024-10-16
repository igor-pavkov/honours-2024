static apr_status_t modsecurity_request_body_start_init(modsec_rec *msr, char **error_msg) {
    *error_msg = NULL;

    if(msr->msc_reqbody_storage == MSC_REQBODY_MEMORY) {
        /* Prepare to store request body in memory. */

        msr->msc_reqbody_chunks = apr_array_make(msr->msc_reqbody_mp,
            32, sizeof(msc_data_chunk *));
        if (msr->msc_reqbody_chunks == NULL) {
            *error_msg = apr_pstrdup(msr->mp, "Input filter: Failed to prepare in-memory storage.");
            return -1;
        }
    } else {
        /* Prepare to store request body on disk. */

        msr->msc_reqbody_filename = apr_psprintf(msr->mp, "%s/%s-%s-request_body-XXXXXX",
            msr->txcfg->tmp_dir, current_filetime(msr->mp), msr->txid);
        if (msr->msc_reqbody_filename == NULL) {
            *error_msg = apr_pstrdup(msr->mp, "Input filter: Failed to generate an on-disk filename.");
            return -1;
        }

        msr->msc_reqbody_fd = msc_mkstemp((char *)msr->msc_reqbody_filename);
        if (msr->msc_reqbody_fd < 0) {
            *error_msg = apr_psprintf(msr->mp, "Input filter: Failed to create temporary file: %s",
                msr->msc_reqbody_filename);
            return -1;
        }

        msr_log(msr, 4, "Input filter: Created temporary file to store request body: %s",
            msr->msc_reqbody_filename);
    }

    return 1;
}