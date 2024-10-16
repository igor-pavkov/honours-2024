static char *wsgi_module_name(apr_pool_t *pool, const char *filename)
{
    char *hash = NULL;
    char *file = NULL;

    /*
     * Calculate a name for the module using the MD5 of its full
     * pathname. This is so that different code files with the
     * same basename are still considered unique. Note that where
     * we believe a case insensitive file system is being used,
     * we always change the file name to lower case so that use
     * of different case in name doesn't result in duplicate
     * modules being loaded for the same file.
     */

    file = (char *)filename;

    if (wsgi_server_config->case_sensitivity) {
        file = apr_pstrdup(pool, file);
        ap_str_tolower(file);
    }

    hash = ap_md5(pool, (const unsigned char *)file);
    return apr_pstrcat(pool, "_mod_wsgi_", hash, NULL);
}