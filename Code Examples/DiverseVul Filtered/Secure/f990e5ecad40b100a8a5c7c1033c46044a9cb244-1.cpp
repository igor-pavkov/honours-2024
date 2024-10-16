static apr_status_t add_trailer(h2_stream *stream,
                                const char *name, size_t nlen,
                                const char *value, size_t vlen,
                                size_t max_field_len, int *pwas_added)
{
    conn_rec *c = stream->session->c;
    char *hname, *hvalue;
    const char *existing;

    *pwas_added = 0;
    if (nlen == 0 || name[0] == ':') {
        ap_log_cerror(APLOG_MARK, APLOG_DEBUG, APR_EINVAL, c, 
                      H2_STRM_LOG(APLOGNO(03060), stream, 
                      "pseudo header in trailer"));
        return APR_EINVAL;
    }
    if (h2_req_ignore_trailer(name, nlen)) {
        return APR_SUCCESS;
    }
    if (!stream->trailers) {
        stream->trailers = apr_table_make(stream->pool, 5);
    }
    hname = apr_pstrndup(stream->pool, name, nlen);
    h2_util_camel_case_header(hname, nlen);
    existing = apr_table_get(stream->trailers, hname);
    if (max_field_len 
        && ((existing? strlen(existing)+2 : 0) + vlen + nlen + 2 > max_field_len)) {
        /* "key: (oldval, )?nval" is too long */
        return APR_EINVAL;
    }
    if (!existing) *pwas_added = 1;
    hvalue = apr_pstrndup(stream->pool, value, vlen);
    apr_table_mergen(stream->trailers, hname, hvalue);
    ap_log_cerror(APLOG_MARK, APLOG_TRACE2, 0, c, 
                  H2_STRM_MSG(stream, "added trailer '%s: %s'"), hname, hvalue);
    
    return APR_SUCCESS;
}