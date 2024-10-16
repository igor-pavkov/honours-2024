static apr_status_t cleanup_tables(void *not_used)
{
    ap_log_error(APLOG_MARK, APLOG_INFO, 0, NULL, APLOGNO(01756)
                  "cleaning up shared memory");

    if (client_rmm) {
        apr_rmm_destroy(client_rmm);
        client_rmm = NULL;
    }

    if (client_shm) {
        apr_shm_destroy(client_shm);
        client_shm = NULL;
    }

    if (client_lock) {
        apr_global_mutex_destroy(client_lock);
        client_lock = NULL;
    }

    if (opaque_lock) {
        apr_global_mutex_destroy(opaque_lock);
        opaque_lock = NULL;
    }

    client_list = NULL;

    return APR_SUCCESS;
}