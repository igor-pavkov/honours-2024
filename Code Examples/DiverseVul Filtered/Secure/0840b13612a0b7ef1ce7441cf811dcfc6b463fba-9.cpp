apr_status_t modsecurity_request_body_retrieve_start(modsec_rec *msr, char **error_msg) {
    *error_msg = NULL;

    if (msr->msc_reqbody_storage == MSC_REQBODY_MEMORY) {
        msr->msc_reqbody_chunk_position = 0;
        msr->msc_reqbody_chunk_offset = 0;

        msr->msc_reqbody_disk_chunk = apr_pcalloc(msr->msc_reqbody_mp, sizeof(msc_data_chunk));
        if (msr->msc_reqbody_disk_chunk == NULL) {
            *error_msg = apr_psprintf(msr->mp, "Failed to allocate %lu bytes for request body disk chunk.",
                (unsigned long)sizeof(msc_data_chunk));
            return -1;
        }
        msr->msc_reqbody_disk_chunk->is_permanent = 1;
    }
    else
    if (msr->msc_reqbody_storage == MSC_REQBODY_DISK) {
        msr->msc_reqbody_disk_chunk = apr_pcalloc(msr->msc_reqbody_mp, sizeof(msc_data_chunk));
        if (msr->msc_reqbody_disk_chunk == NULL) {
            *error_msg = apr_psprintf(msr->mp, "Failed to allocate %lu bytes for request body disk chunk.",
                (unsigned long)sizeof(msc_data_chunk));
            return -1;
        }

        msr->msc_reqbody_disk_chunk->is_permanent = 0;
        msr->msc_reqbody_disk_chunk->data = apr_palloc(msr->msc_reqbody_mp, CHUNK_CAPACITY);
        if (msr->msc_reqbody_disk_chunk->data == NULL) {
            *error_msg = apr_psprintf(msr->mp, "Failed to allocate %d bytes for request body disk chunk data.",
                CHUNK_CAPACITY);
            return -1;
        }

        msr->msc_reqbody_fd = open(msr->msc_reqbody_filename, O_RDONLY | O_BINARY);
        if (msr->msc_reqbody_fd < 0) {
            *error_msg = apr_psprintf(msr->mp, "Failed to open temporary file for reading: %s",
                msr->msc_reqbody_filename);
            return -1;
        }
    }

    return 1;
}