static const char *new_digest(const request_rec *r,
                              digest_header_rec *resp)
{
    const char *ha1, *ha2, *a2;

    ha1 = resp->ha1;

    a2 = apr_pstrcat(r->pool, resp->method, ":", resp->uri, NULL);
    ha2 = ap_md5(r->pool, (const unsigned char *)a2);

    return ap_md5(r->pool,
                  (unsigned char *)apr_pstrcat(r->pool, ha1, ":", resp->nonce,
                                               ":", resp->nonce_count, ":",
                                               resp->cnonce, ":",
                                               resp->message_qop, ":", ha2,
                                               NULL));
}