apr_status_t h2_stream_add_header(h2_stream *stream,
                                  const char *name, size_t nlen,
                                  const char *value, size_t vlen)
{
    h2_session *session = stream->session;
    int error = 0, was_added = 0;
    apr_status_t status = APR_SUCCESS;
    
    if (stream->has_response) {
        return APR_EINVAL;    
    }

    if (name[0] == ':') {
        if ((vlen) > session->s->limit_req_line) {
            /* pseudo header: approximation of request line size check */
            if (!h2_stream_is_ready(stream)) {
                ap_log_cerror(APLOG_MARK, APLOG_INFO, 0, session->c,
                              H2_STRM_LOG(APLOGNO(10178), stream,
                                          "Request pseudo header exceeds "
                                          "LimitRequestFieldSize: %s"), name);
            }
            error = HTTP_REQUEST_URI_TOO_LARGE;
            goto cleanup;
        }
    }
    
    if (session->s->limit_req_fields > 0 
        && stream->request_headers_added > session->s->limit_req_fields) {
        /* already over limit, count this attempt, but do not take it in */
        ++stream->request_headers_added;
    }
    else if (H2_SS_IDLE == stream->state) {
        if (!stream->rtmp) {
            stream->rtmp = h2_req_create(stream->id, stream->pool, 
                                         NULL, NULL, NULL, NULL, NULL, 0);
        }
        status = h2_request_add_header(stream->rtmp, stream->pool,
                                       name, nlen, value, vlen,
                                       session->s->limit_req_fieldsize, &was_added);
        if (was_added) ++stream->request_headers_added;
    }
    else if (H2_SS_OPEN == stream->state) {
        status = add_trailer(stream, name, nlen, value, vlen,
                             session->s->limit_req_fieldsize, &was_added);
        if (was_added) ++stream->request_headers_added;
    }
    else {
        status = APR_EINVAL;
        goto cleanup;
    }
    
    if (APR_EINVAL == status) {
        /* header too long */
        if (!h2_stream_is_ready(stream)) {
            ap_log_cerror(APLOG_MARK, APLOG_INFO, 0, session->c,
                          H2_STRM_LOG(APLOGNO(10180), stream,"Request header exceeds "
                                      "LimitRequestFieldSize: %.*s"),
                          (int)H2MIN(nlen, 80), name);
        }
        error = HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE;
        goto cleanup;
    }
    
    if (session->s->limit_req_fields > 0 
        && stream->request_headers_added > session->s->limit_req_fields) {
        /* too many header lines */
        if (stream->request_headers_added > session->s->limit_req_fields + 100) {
            /* yeah, right, this request is way over the limit, say goodbye */
            h2_stream_rst(stream, H2_ERR_ENHANCE_YOUR_CALM);
            return APR_ECONNRESET;
        }
        if (!h2_stream_is_ready(stream)) {
            ap_log_cerror(APLOG_MARK, APLOG_INFO, 0, session->c,
                          H2_STRM_LOG(APLOGNO(10181), stream, "Number of request headers "
                                      "exceeds LimitRequestFields"));
        }
        error = HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE;
        goto cleanup;
    }
    
cleanup:
    if (error) {
        set_error_response(stream, error);
        return APR_EINVAL; 
    }
    else if (status != APR_SUCCESS) {
        ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, session->c,
                      H2_STRM_MSG(stream, "header %s not accepted"), name);
        h2_stream_dispatch(stream, H2_SEV_CANCELLED);
    }
    return status;
}