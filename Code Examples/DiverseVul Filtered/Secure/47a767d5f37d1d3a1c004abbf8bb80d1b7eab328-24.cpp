const apr_array_header_t *am_cond_backrefs(request_rec *r, 
                                           const am_cond_t *ce, 
                                           const char *value, 
                                           const ap_regmatch_t *regmatch)
{
    apr_array_header_t *backrefs;
    const char **ref;
    int nsub;
    int i;

    nsub = ce->regex->re_nsub + 1;     /* +1 for %0 */
    backrefs = apr_array_make(r->pool, nsub, sizeof(const char *));
    backrefs->nelts = nsub;

    ref = (const char **)(backrefs->elts);

    for (i = 0; i < nsub; i++) {
        if ((regmatch[i].rm_so == -1) || (regmatch[i].rm_eo == -1)) {
            ref[i] = "";
        } else {
            int len = regmatch[i].rm_eo - regmatch[i].rm_so;
            int off = regmatch[i].rm_so;

            ref[i] = apr_pstrndup(r->pool, value + off, len);
        }
    }

    return (const apr_array_header_t *)backrefs;
}