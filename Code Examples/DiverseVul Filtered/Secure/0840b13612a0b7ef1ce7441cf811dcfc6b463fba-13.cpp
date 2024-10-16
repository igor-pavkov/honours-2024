apr_status_t modsecurity_request_body_retrieve(modsec_rec *msr,
    msc_data_chunk **chunk, long int nbytes, char **error_msg)
{
    msc_data_chunk **chunks;

    *error_msg = NULL;

    if (chunk == NULL) {
        *error_msg = apr_pstrdup(msr->mp, "Internal error, retrieving request body chunk.");
        return -1;
    }
    *chunk = NULL;

    if (msr->msc_reqbody_storage == MSC_REQBODY_MEMORY) {
        /* Are there any chunks left? */
        if (msr->msc_reqbody_chunk_position >= msr->msc_reqbody_chunks->nelts) {
            /* No more chunks. */
            return 0;
        }

        /* We always respond with the same chunk, just different information in it. */
        *chunk = msr->msc_reqbody_disk_chunk;

        /* Advance to the current chunk and position on the
         * next byte we need to send.
         */
        chunks = (msc_data_chunk **)msr->msc_reqbody_chunks->elts;
        msr->msc_reqbody_disk_chunk->data = chunks[msr->msc_reqbody_chunk_position]->data
            + msr->msc_reqbody_chunk_offset;

        if (nbytes < 0) {
            /* Send what's left in this chunk as there is no limit on the size. */
            msr->msc_reqbody_disk_chunk->length = chunks[msr->msc_reqbody_chunk_position]->length;
            msr->msc_reqbody_chunk_position++;
            msr->msc_reqbody_chunk_offset = 0;
        } else {
            /* We have a limit we must obey. */

            if (chunks[msr->msc_reqbody_chunk_position]->length -
                msr->msc_reqbody_chunk_offset <= (unsigned int)nbytes)
            {
                /* If what's left in our chunk is less than the limit
                 * then send it all back.
                 */
                msr->msc_reqbody_disk_chunk->length =
                    chunks[msr->msc_reqbody_chunk_position]->length -
                    msr->msc_reqbody_chunk_offset;
                msr->msc_reqbody_chunk_position++;
                msr->msc_reqbody_chunk_offset = 0;
            } else {
                /* If we have more data in our chunk, send the
                 * maximum bytes we can (nbytes).
                 */
                msr->msc_reqbody_disk_chunk->length = nbytes;
                msr->msc_reqbody_chunk_offset += nbytes;
            }
        }

        /* If we've advanced beyond our last chunk then
         * we have no more data to send.
         */
        if (msr->msc_reqbody_chunk_position >= msr->msc_reqbody_chunks->nelts) {
            return 0; /* No more chunks. */
        }

        /* More data available. */
        return 1;
    }

    if (msr->msc_reqbody_storage == MSC_REQBODY_DISK) {
        long int my_nbytes = CHUNK_CAPACITY;
        int i;

        /* Send CHUNK_CAPACITY bytes at a time unless a lower limit was requested. */
        if ((nbytes != -1)&&(my_nbytes > nbytes)) {
            my_nbytes = nbytes;
        }

        i = read(msr->msc_reqbody_fd, msr->msc_reqbody_disk_chunk->data, my_nbytes);
        if (i < 0) {
            *error_msg = apr_psprintf(msr->mp, "Input filter: Error reading from temporary file: %s",
                strerror(errno));
            return -1;
        }

        *chunk = msr->msc_reqbody_disk_chunk;
        msr->msc_reqbody_disk_chunk->length = i;

        if (i == 0) return 0; /* No more data available. */

        return 1; /* More data available. */
    }

    /* Should never happen. */
    *error_msg = apr_psprintf(msr->mp, "Internal error, invalid msc_reqbody_storage value: %u",
        msr->msc_reqbody_storage);

    return -1;
}