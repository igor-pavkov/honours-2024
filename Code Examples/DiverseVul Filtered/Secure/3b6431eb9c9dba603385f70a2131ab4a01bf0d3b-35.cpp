static int pre_init(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp)
{
    apr_status_t rv;
    void *retained;

    rv = ap_mutex_register(pconf, client_mutex_type, NULL, APR_LOCK_DEFAULT, 0);
    if (rv != APR_SUCCESS)
        return !OK;
    rv = ap_mutex_register(pconf, opaque_mutex_type, NULL, APR_LOCK_DEFAULT, 0);
    if (rv != APR_SUCCESS)
        return !OK;

    retained = ap_retained_data_get(RETAINED_DATA_ID);
    if (retained == NULL) {
        retained = ap_retained_data_create(RETAINED_DATA_ID, SECRET_LEN);
        ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, APLOGNO(01757)
                     "generating secret for digest authentication");
#if APR_HAS_RANDOM
        rv = apr_generate_random_bytes(retained, SECRET_LEN);
#else
#error APR random number support is missing
#endif
        if (rv != APR_SUCCESS) {
            ap_log_error(APLOG_MARK, APLOG_CRIT, rv, NULL, APLOGNO(01758)
                         "error generating secret");
            return !OK;
        }
    }
    secret = retained;
    return OK;
}