static apr_status_t modsecurity_request_body_end_raw(modsec_rec *msr, char **error_msg) {
    msc_data_chunk **chunks, *one_chunk;
    char *d;
    int i, sofar;

    *error_msg = NULL;

    /* Allocate a buffer large enough to hold the request body. */

    if (msr->msc_reqbody_length + 1 == 0) {
        *error_msg = apr_psprintf(msr->mp, "Internal error, request body length will overflow: %u",
            msr->msc_reqbody_length);
        return -1;
    }

    msr->msc_reqbody_buffer = malloc(msr->msc_reqbody_length + 1);
    if (msr->msc_reqbody_buffer == NULL) {
        *error_msg = apr_psprintf(msr->mp, "Unable to allocate memory to hold request body. Asked for %u bytes.",
            msr->msc_reqbody_length + 1);
        return -1;
    }

    msr->msc_reqbody_buffer[msr->msc_reqbody_length] = '\0';

    /* Copy the data we keep in chunks into the new buffer. */

    sofar = 0;
    d = msr->msc_reqbody_buffer;
    chunks = (msc_data_chunk **)msr->msc_reqbody_chunks->elts;
    for(i = 0; i < msr->msc_reqbody_chunks->nelts; i++) {
        if (sofar + chunks[i]->length <= msr->msc_reqbody_length) {
            memcpy(d, chunks[i]->data, chunks[i]->length);
            d += chunks[i]->length;
            sofar += chunks[i]->length;
        } else {
            *error_msg = apr_psprintf(msr->mp, "Internal error, request body buffer overflow.");
            return -1;
        }
    }


    /* Now free the memory used by the chunks. */

    chunks = (msc_data_chunk **)msr->msc_reqbody_chunks->elts;
    for(i = 0; i < msr->msc_reqbody_chunks->nelts; i++) {
        free(chunks[i]->data);
        chunks[i]->data = NULL;
    }

    /* Create a new array with only one chunk in it. */

    msr->msc_reqbody_chunks = apr_array_make(msr->msc_reqbody_mp, 2, sizeof(msc_data_chunk *));
    if (msr->msc_reqbody_chunks == NULL) {
        *error_msg = apr_pstrdup(msr->mp, "Failed to create structure to hold request body.");
        return -1;
    }

    one_chunk = (msc_data_chunk *)apr_pcalloc(msr->msc_reqbody_mp, sizeof(msc_data_chunk));
    one_chunk->data = msr->msc_reqbody_buffer;
    one_chunk->length = msr->msc_reqbody_length;
    one_chunk->is_permanent = 1;
    *(const msc_data_chunk **)apr_array_push(msr->msc_reqbody_chunks) = one_chunk;

    if(msr->txcfg->reqbody_limit > 0 && msr->txcfg->reqbody_limit < msr->msc_reqbody_length)    {
        msr->msc_reqbody_length = msr->txcfg->reqbody_limit;
    }

    return 1;
}