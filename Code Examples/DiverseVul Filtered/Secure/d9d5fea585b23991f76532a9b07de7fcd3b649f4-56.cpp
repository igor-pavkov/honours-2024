static apr_status_t apr_os_pipe_put_ex(apr_file_t **file,
                                       apr_os_file_t *thefile,
                                       int register_cleanup,
                                       apr_pool_t *pool)
{
    apr_status_t rv;

    rv = apr_os_pipe_put(file, thefile, pool);

    if (register_cleanup) {
        apr_pool_cleanup_register(pool, (void *)(*file),
                                  apr_unix_file_cleanup,
                                  apr_pool_cleanup_null);
    }

    return rv;
}