static long gc(server_rec *s)
{
    client_entry *entry, *prev;
    unsigned long num_removed = 0, idx;

    /* garbage collect all last entries */

    for (idx = 0; idx < client_list->tbl_len; idx++) {
        entry = client_list->table[idx];
        prev  = NULL;

        if (!entry) {
            /* This bucket is empty. */
            continue;
        }

        while (entry->next) {   /* find last entry */
            prev  = entry;
            entry = entry->next;
        }
        if (prev) {
            prev->next = NULL;   /* cut list */
        }
        else {
            client_list->table[idx] = NULL;
        }
        if (entry) {                    /* remove entry */
            apr_status_t err;

            err = rmm_free(client_rmm, entry);
            num_removed++;

            if (err) {
                /* Nothing we can really do but log... */
                ap_log_error(APLOG_MARK, APLOG_ERR, err, s, APLOGNO(10007)
                             "Failed to free auth_digest client allocation");
            }
        }
    }

    /* update counters and log */

    client_list->num_entries -= num_removed;
    client_list->num_removed += num_removed;

    return num_removed;
}