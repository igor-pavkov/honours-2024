static void initialize_child(apr_pool_t *p, server_rec *s)
{
    apr_status_t sts;

    if (!client_shm) {
        return;
    }

    /* Get access to rmm in child */
    sts = apr_rmm_attach(&client_rmm,
                         NULL,
                         apr_shm_baseaddr_get(client_shm),
                         p);
    if (sts != APR_SUCCESS) {
        log_error_and_cleanup("failed to attach to rmm", sts, s);
        return;
    }

    sts = apr_global_mutex_child_init(&client_lock,
                                      apr_global_mutex_lockfile(client_lock),
                                      p);
    if (sts != APR_SUCCESS) {
        log_error_and_cleanup("failed to create lock (client_lock)", sts, s);
        return;
    }
    sts = apr_global_mutex_child_init(&opaque_lock,
                                      apr_global_mutex_lockfile(opaque_lock),
                                      p);
    if (sts != APR_SUCCESS) {
        log_error_and_cleanup("failed to create lock (opaque_lock)", sts, s);
        return;
    }
}