static apr_status_t rmm_free(apr_rmm_t *rmm, void *alloc)
{
    apr_rmm_off_t offset = apr_rmm_offset_get(rmm, alloc);

    return apr_rmm_free(rmm, offset);
}