AP_DECLARE(void) ap_set_sub_req_protocol(request_rec *rnew,
                                         const request_rec *r)
{
    rnew->the_request     = r->the_request;  /* Keep original request-line */

    rnew->assbackwards    = 1;   /* Don't send headers from this. */
    rnew->no_local_copy   = 1;   /* Don't try to send HTTP_NOT_MODIFIED for a
                                  * fragment. */
    rnew->method          = "GET";
    rnew->method_number   = M_GET;
    rnew->protocol        = "INCLUDED";

    rnew->status          = HTTP_OK;

    rnew->headers_in      = apr_table_copy(rnew->pool, r->headers_in);
    rnew->trailers_in     = apr_table_copy(rnew->pool, r->trailers_in);

    /* did the original request have a body?  (e.g. POST w/SSI tags)
     * if so, make sure the subrequest doesn't inherit body headers
     */
    if (!r->kept_body && (apr_table_get(r->headers_in, "Content-Length")
        || apr_table_get(r->headers_in, "Transfer-Encoding"))) {
        strip_headers_request_body(rnew);
    }
    rnew->subprocess_env  = apr_table_copy(rnew->pool, r->subprocess_env);
    rnew->headers_out     = apr_table_make(rnew->pool, 5);
    rnew->err_headers_out = apr_table_make(rnew->pool, 5);
    rnew->trailers_out    = apr_table_make(rnew->pool, 5);
    rnew->notes           = apr_table_make(rnew->pool, 5);

    rnew->expecting_100   = r->expecting_100;
    rnew->read_length     = r->read_length;
    rnew->read_body       = REQUEST_NO_BODY;

    rnew->main = (request_rec *) r;
}