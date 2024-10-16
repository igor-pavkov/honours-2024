static client_entry *get_client(unsigned long key, const request_rec *r)
{
    int bucket;
    client_entry *entry, *prev = NULL;


    if (!key || !client_shm)  return NULL;

    bucket = key % client_list->tbl_len;
    entry  = client_list->table[bucket];

    apr_global_mutex_lock(client_lock);

    while (entry && key != entry->key) {
        prev  = entry;
        entry = entry->next;
    }

    if (entry && prev) {                /* move entry to front of list */
        prev->next  = entry->next;
        entry->next = client_list->table[bucket];
        client_list->table[bucket] = entry;
    }

    apr_global_mutex_unlock(client_lock);

    if (entry) {
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(01764)
                      "get_client(): client %lu found", key);
    }
    else {
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(01765)
                      "get_client(): client %lu not found", key);
    }

    return entry;
}