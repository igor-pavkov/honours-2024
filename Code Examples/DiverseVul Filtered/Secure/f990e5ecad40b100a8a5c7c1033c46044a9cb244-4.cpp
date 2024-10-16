apr_status_t h2_stream_out_prepare(h2_stream *stream, apr_off_t *plen, 
                                   int *peos, h2_headers **pheaders)
{
    apr_status_t status = APR_SUCCESS;
    apr_off_t requested, missing, max_chunk = H2_DATA_CHUNK_SIZE;
    conn_rec *c;
    int complete, was_closed = 0;

    ap_assert(stream);
    
    if (stream->rst_error) {
        *plen = 0;
        *peos = 1;
        return APR_ECONNRESET;
    }
    
    c = stream->session->c;
    prep_output(stream);

    /* determine how much we'd like to send. We cannot send more than
     * is requested. But we can reduce the size in case the master
     * connection operates in smaller chunks. (TSL warmup) */
    if (stream->session->io.write_size > 0) {
        max_chunk = stream->session->io.write_size - H2_FRAME_HDR_LEN; 
    }
    requested = (*plen > 0)? H2MIN(*plen, max_chunk) : max_chunk;
    
    /* count the buffered data until eos or a headers bucket */
    status = add_buffered_data(stream, requested, plen, peos, &complete, pheaders);
    
    if (status == APR_EAGAIN) {
        /* TODO: ugly, someone needs to retrieve the response first */
        h2_mplx_m_keep_active(stream->session->mplx, stream);
        ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, c,
                      H2_STRM_MSG(stream, "prep, response eagain"));
        return status;
    }
    else if (status != APR_SUCCESS) {
        return status;
    }
    
    if (pheaders && *pheaders) {
        return APR_SUCCESS;
    }
    
    /* If there we do not have enough buffered data to satisfy the requested
     * length *and* we counted the _complete_ buffer (and did not stop in the middle
     * because of meta data there), lets see if we can read more from the
     * output beam */
    missing = H2MIN(requested, stream->max_mem) - *plen;
    if (complete && !*peos && missing > 0) {
        apr_status_t rv = APR_EOF;
        
        if (stream->output) {
            H2_STREAM_OUT_LOG(APLOG_TRACE2, stream, "pre");
            h2_beam_log(stream->output, c, APLOG_TRACE2, "pre read output");
            rv = h2_beam_receive(stream->output, stream->out_buffer,
                                 APR_NONBLOCK_READ, stream->max_mem - *plen, &was_closed);
            H2_STREAM_OUT_LOG(APLOG_TRACE2, stream, "post");
            h2_beam_log(stream->output, c, APLOG_TRACE2, "post read output");
        }
        
        if (rv == APR_SUCCESS) {
            /* count the buffer again, now that we have read output */
            status = add_buffered_data(stream, requested, plen, peos, &complete, pheaders);
        }
        else if (APR_STATUS_IS_EOF(rv)) {
            apr_bucket *eos = apr_bucket_eos_create(c->bucket_alloc);
            APR_BRIGADE_INSERT_TAIL(stream->out_buffer, eos);
            *peos = 1;
        }
        else if (APR_STATUS_IS_EAGAIN(rv)) {
            /* we set this is the status of this call only if there
             * is no buffered data, see check below */
        }
        else {
            /* real error reading. Give this back directly, even though
             * we may have something buffered. */
            status = rv;
        }
    }
    
    if (status == APR_SUCCESS) {
        if (*peos || *plen) {
            ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, c,
                          H2_STRM_MSG(stream, "prepare, len=%ld eos=%d"),
                          (long)*plen, *peos);
        }
        else {
            status = was_closed? APR_EOF : APR_EAGAIN;
            ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, c,
                          H2_STRM_MSG(stream, "prepare, no data"));
        }
    }
    return status;
}