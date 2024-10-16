enum store_item_type do_store_item(item *it, int comm, conn *c, const uint32_t hv) {
    char *key = ITEM_key(it);
    item *old_it = do_item_get(key, it->nkey, hv);
    enum store_item_type stored = NOT_STORED;

    item *new_it = NULL;
    int flags;

    if (old_it != NULL && comm == NREAD_ADD) {
        /* add only adds a nonexistent item, but promote to head of LRU */
        do_item_update(old_it);
    } else if (!old_it && (comm == NREAD_REPLACE
        || comm == NREAD_APPEND || comm == NREAD_PREPEND))
    {
        /* replace only replaces an existing value; don't store */
    } else if (comm == NREAD_CAS) {
        /* validate cas operation */
        if(old_it == NULL) {
            // LRU expired
            stored = NOT_FOUND;
            pthread_mutex_lock(&c->thread->stats.mutex);
            c->thread->stats.cas_misses++;
            pthread_mutex_unlock(&c->thread->stats.mutex);
        }
        else if (ITEM_get_cas(it) == ITEM_get_cas(old_it)) {
            // cas validates
            // it and old_it may belong to different classes.
            // I'm updating the stats for the one that's getting pushed out
            pthread_mutex_lock(&c->thread->stats.mutex);
            c->thread->stats.slab_stats[old_it->slabs_clsid].cas_hits++;
            pthread_mutex_unlock(&c->thread->stats.mutex);

            item_replace(old_it, it, hv);
            stored = STORED;
        } else {
            pthread_mutex_lock(&c->thread->stats.mutex);
            c->thread->stats.slab_stats[old_it->slabs_clsid].cas_badval++;
            pthread_mutex_unlock(&c->thread->stats.mutex);

            if(settings.verbose > 1) {
                fprintf(stderr, "CAS:  failure: expected %llu, got %llu\n",
                        (unsigned long long)ITEM_get_cas(old_it),
                        (unsigned long long)ITEM_get_cas(it));
            }
            stored = EXISTS;
        }
    } else {
        /*
         * Append - combine new and old record into single one. Here it's
         * atomic and thread-safe.
         */
        if (comm == NREAD_APPEND || comm == NREAD_PREPEND) {
            /*
             * Validate CAS
             */
            if (ITEM_get_cas(it) != 0) {
                // CAS much be equal
                if (ITEM_get_cas(it) != ITEM_get_cas(old_it)) {
                    stored = EXISTS;
                }
            }

            if (stored == NOT_STORED) {
                /* we have it and old_it here - alloc memory to hold both */
                /* flags was already lost - so recover them from ITEM_suffix(it) */

                flags = (int) strtol(ITEM_suffix(old_it), (char **) NULL, 10);

                new_it = do_item_alloc(key, it->nkey, flags, old_it->exptime, it->nbytes + old_it->nbytes - 2 /* CRLF */, hv);

                if (new_it == NULL) {
                    /* SERVER_ERROR out of memory */
                    if (old_it != NULL)
                        do_item_remove(old_it);

                    return NOT_STORED;
                }

                /* copy data from it and old_it to new_it */

                if (comm == NREAD_APPEND) {
                    memcpy(ITEM_data(new_it), ITEM_data(old_it), old_it->nbytes);
                    memcpy(ITEM_data(new_it) + old_it->nbytes - 2 /* CRLF */, ITEM_data(it), it->nbytes);
                } else {
                    /* NREAD_PREPEND */
                    memcpy(ITEM_data(new_it), ITEM_data(it), it->nbytes);
                    memcpy(ITEM_data(new_it) + it->nbytes - 2 /* CRLF */, ITEM_data(old_it), old_it->nbytes);
                }

                it = new_it;
            }
        }

        if (stored == NOT_STORED) {
            if (old_it != NULL)
                item_replace(old_it, it, hv);
            else
                do_item_link(it, hv);

            c->cas = ITEM_get_cas(it);

            stored = STORED;
        }
    }

    if (old_it != NULL)
        do_item_remove(old_it);         /* release our reference */
    if (new_it != NULL)
        do_item_remove(new_it);

    if (stored == STORED) {
        c->cas = ITEM_get_cas(it);
    }

    return stored;
}