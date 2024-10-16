AP_DECLARE(apr_status_t) ap_rgetline(char **s, apr_size_t n,
                                     apr_size_t *read, request_rec *r,
                                     int fold, apr_bucket_brigade *bb)
{
    /* on ASCII boxes, ap_rgetline is a macro which simply invokes
     * ap_rgetline_core with the same parms
     *
     * on EBCDIC boxes, each complete http protocol input line needs to be
     * translated into the code page used by the compiler.  Since
     * ap_rgetline_core uses recursion, we do the translation in a wrapper
     * function to ensure that each input character gets translated only once.
     */
    apr_status_t rv;

    rv = ap_rgetline_core(s, n, read, r, fold, bb);
    if (rv == APR_SUCCESS) {
        ap_xlate_proto_from_ascii(*s, *read);
    }
    return rv;
}