static void *rmm_malloc(apr_rmm_t *rmm, apr_size_t size)
{
    apr_rmm_off_t offset = apr_rmm_malloc(rmm, size);

    if (!offset) {
        return NULL;
    }

    return apr_rmm_addr_get(rmm, offset);
}