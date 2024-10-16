int am_postdir_cleanup(request_rec *r)
{
    am_mod_cfg_rec *mod_cfg;
    apr_dir_t *postdir;
    apr_status_t rv;
    char error_buffer[64];
    apr_finfo_t afi;
    char *fname;
    int count;
    apr_time_t expire_before;

    mod_cfg = am_get_mod_cfg(r->server);

    /* The oldes file we should keep. Delete files that are older. */
    expire_before = apr_time_now() - mod_cfg->post_ttl * APR_USEC_PER_SEC;

    /*
     * Open our POST directory or create it. 
     */
    rv = apr_dir_open(&postdir, mod_cfg->post_dir, r->pool);
    if (rv != 0) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                      "Unable to open MellonPostDirectory \"%s\": %s",
                      mod_cfg->post_dir,
                      apr_strerror(rv, error_buffer, sizeof(error_buffer)));
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    /*
     * Purge outdated items
     */
    count = 0;
    do {
        rv = apr_dir_read(&afi, APR_FINFO_NAME|APR_FINFO_CTIME, postdir);
        if (rv != OK)
            break;

        /* Skip dot_files */
        if (afi.name[0] == '.')
             continue;

        if (afi.ctime < expire_before) {
            fname = apr_psprintf(r->pool, "%s/%s", mod_cfg->post_dir, afi.name);
            (void)apr_file_remove(fname , r->pool); 
        } else {
            count++;
        }
    } while (1 /* CONSTCOND */);

    (void)apr_dir_close(postdir);

    if (count >= mod_cfg->post_count) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, 
                      "Too many saved POST sessions. "
                      "Increase MellonPostCount directive.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return OK;
}