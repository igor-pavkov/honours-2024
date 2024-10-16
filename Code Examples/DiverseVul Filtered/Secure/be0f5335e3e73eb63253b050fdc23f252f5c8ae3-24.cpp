AP_DECLARE(void) ap_set_last_modified(request_rec *r)
{
    if (!r->assbackwards) {
        apr_time_t mod_time = ap_rationalize_mtime(r, r->mtime);
        char *datestr = apr_palloc(r->pool, APR_RFC822_DATE_LEN);

        apr_rfc822_date(datestr, mod_time);
        apr_table_setn(r->headers_out, "Last-Modified", datestr);
    }
}