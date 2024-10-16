int am_validate_redirect_url(request_rec *r, const char *url)
{
    am_dir_cfg_rec *cfg = am_get_dir_cfg(r);
    apr_uri_t uri;
    int ret;

    ret = apr_uri_parse(r->pool, url, &uri);
    if (ret != APR_SUCCESS) {
        AM_LOG_RERROR(APLOG_MARK, APLOG_ERR, 0, r,
                      "Invalid redirect URL: %s", url);
        return HTTP_BAD_REQUEST;
    }

    /* Sanity check of the scheme of the domain. We only allow http and https. */
    if (uri.scheme) {
        /* http and https schemes without hostname are invalid. */
        if (!uri.hostname) {
            AM_LOG_RERROR(APLOG_MARK, APLOG_ERR, 0, r,
                          "Preventing redirect with scheme but no hostname: %s",
                          url);
            return HTTP_BAD_REQUEST;
        }
        if (strcasecmp(uri.scheme, "http")
            && strcasecmp(uri.scheme, "https")) {
            AM_LOG_RERROR(APLOG_MARK, APLOG_ERR, 0, r,
                          "Only http or https scheme allowed in redirect URL: %s (%s)",
                          url, uri.scheme);
            return HTTP_BAD_REQUEST;
        }
    }

    if (!uri.hostname) {
        return OK; /* No hostname to check. */
    }

    for (int i = 0; cfg->redirect_domains[i] != NULL; i++) {
        const char *redirect_domain = cfg->redirect_domains[i];
        if (!strcasecmp(redirect_domain, "[self]")) {
            if (!strcasecmp(uri.hostname, am_request_hostname(r))) {
                return OK;
            }
        } else if (apr_fnmatch(redirect_domain, uri.hostname,
                               APR_FNM_PERIOD | APR_FNM_CASE_BLIND) ==
                   APR_SUCCESS) {
            return OK;
        }
    }
    AM_LOG_RERROR(APLOG_MARK, APLOG_ERR, 0, r,
                  "Untrusted hostname (%s) in redirect URL: %s",
                  uri.hostname, url);
    return HTTP_BAD_REQUEST;
}