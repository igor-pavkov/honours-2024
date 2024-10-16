static int initialize_tables(server_rec *s, apr_pool_t *ctx)
{
    unsigned long idx;
    apr_status_t   sts;

    /* set up client list */

    /* Create the shared memory segment */

    client_shm = NULL;
    client_rmm = NULL;
    client_lock = NULL;
    opaque_lock = NULL;
    client_list = NULL;

    /*
     * Create a unique filename using our pid. This information is
     * stashed in the global variable so the children inherit it.
     */
    client_shm_filename = ap_runtime_dir_relative(ctx, "authdigest_shm");
    client_shm_filename = ap_append_pid(ctx, client_shm_filename, ".");

    /* Use anonymous shm by default, fall back on name-based. */
    sts = apr_shm_create(&client_shm, shmem_size, NULL, ctx);
    if (APR_STATUS_IS_ENOTIMPL(sts)) {
        /* For a name-based segment, remove it first in case of a
         * previous unclean shutdown. */
        apr_shm_remove(client_shm_filename, ctx);

        /* Now create that segment */
        sts = apr_shm_create(&client_shm, shmem_size,
                            client_shm_filename, ctx);
    }

    if (APR_SUCCESS != sts) {
        ap_log_error(APLOG_MARK, APLOG_ERR, sts, s, APLOGNO(01762)
                     "Failed to create shared memory segment on file %s",
                     client_shm_filename);
        log_error_and_cleanup("failed to initialize shm", sts, s);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    sts = apr_rmm_init(&client_rmm,
                       NULL, /* no lock, we'll do the locking ourselves */
                       apr_shm_baseaddr_get(client_shm),
                       shmem_size, ctx);
    if (sts != APR_SUCCESS) {
        log_error_and_cleanup("failed to initialize rmm", sts, s);
        return !OK;
    }

    client_list = rmm_malloc(client_rmm, sizeof(*client_list) +
                                         sizeof(client_entry *) * num_buckets);
    if (!client_list) {
        log_error_and_cleanup("failed to allocate shared memory", -1, s);
        return !OK;
    }
    client_list->table = (client_entry**) (client_list + 1);
    for (idx = 0; idx < num_buckets; idx++) {
        client_list->table[idx] = NULL;
    }
    client_list->tbl_len     = num_buckets;
    client_list->num_entries = 0;

    sts = ap_global_mutex_create(&client_lock, NULL, client_mutex_type, NULL,
                                 s, ctx, 0);
    if (sts != APR_SUCCESS) {
        log_error_and_cleanup("failed to create lock (client_lock)", sts, s);
        return !OK;
    }


    /* setup opaque */

    opaque_cntr = rmm_malloc(client_rmm, sizeof(*opaque_cntr));
    if (opaque_cntr == NULL) {
        log_error_and_cleanup("failed to allocate shared memory", -1, s);
        return !OK;
    }
    *opaque_cntr = 1UL;

    sts = ap_global_mutex_create(&opaque_lock, NULL, opaque_mutex_type, NULL,
                                 s, ctx, 0);
    if (sts != APR_SUCCESS) {
        log_error_and_cleanup("failed to create lock (opaque_lock)", sts, s);
        return !OK;
    }


    /* setup one-time-nonce counter */

    otn_counter = rmm_malloc(client_rmm, sizeof(*otn_counter));
    if (otn_counter == NULL) {
        log_error_and_cleanup("failed to allocate shared memory", -1, s);
        return !OK;
    }
    *otn_counter = 0;
    /* no lock here */


    /* success */
    return OK;
}