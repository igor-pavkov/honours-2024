static const char *old_digest(const request_rec *r,
                              const digest_header_rec *resp)
{
    const char *ha2;

    ha2 = ap_md5(r->pool, (unsigned char *)apr_pstrcat(r->pool, resp->method, ":",
                                                       resp->uri, NULL));
    return ap_md5(r->pool,
                  (unsigned char *)apr_pstrcat(r->pool, resp->ha1, ":",
                                               resp->nonce, ":", ha2, NULL));
}