static client_entry *gen_client(const request_rec *r)
{
    unsigned long op;
    client_entry new_entry = { 0, NULL, 0, "" }, *entry;

    if (!opaque_cntr) {
        return NULL;
    }

    apr_global_mutex_lock(opaque_lock);
    op = (*opaque_cntr)++;
    apr_global_mutex_unlock(opaque_lock);

    if (!(entry = add_client(op, &new_entry, r->server))) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(01769)
                      "failed to allocate client entry - ignoring client");
        return NULL;
    }

    return entry;
}