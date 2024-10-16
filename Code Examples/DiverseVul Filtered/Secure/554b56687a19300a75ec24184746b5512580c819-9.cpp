static void recache_or_free(conn *c, io_wrap *wrap) {
    item *it;
    it = (item *)wrap->io.buf;
    bool do_free = true;
    if (wrap->active) {
        // If request never dispatched, free the read buffer but leave the
        // item header alone.
        do_free = false;
        size_t ntotal = ITEM_ntotal(wrap->hdr_it);
        slabs_free(it, ntotal, slabs_clsid(ntotal));
        c->io_wrapleft--;
        assert(c->io_wrapleft >= 0);
        pthread_mutex_lock(&c->thread->stats.mutex);
        c->thread->stats.get_aborted_extstore++;
        pthread_mutex_unlock(&c->thread->stats.mutex);
    } else if (wrap->miss) {
        // If request was ultimately a miss, unlink the header.
        do_free = false;
        size_t ntotal = ITEM_ntotal(wrap->hdr_it);
        item_unlink(wrap->hdr_it);
        slabs_free(it, ntotal, slabs_clsid(ntotal));
        pthread_mutex_lock(&c->thread->stats.mutex);
        c->thread->stats.miss_from_extstore++;
        if (wrap->badcrc)
            c->thread->stats.badcrc_from_extstore++;
        pthread_mutex_unlock(&c->thread->stats.mutex);
    } else if (settings.ext_recache_rate) {
        // hashvalue is cuddled during store
        uint32_t hv = (uint32_t)it->time;
        // opt to throw away rather than wait on a lock.
        void *hold_lock = item_trylock(hv);
        if (hold_lock != NULL) {
            item *h_it = wrap->hdr_it;
            uint8_t flags = ITEM_LINKED|ITEM_FETCHED|ITEM_ACTIVE;
            // Item must be recently hit at least twice to recache.
            if (((h_it->it_flags & flags) == flags) &&
                    h_it->time > current_time - ITEM_UPDATE_INTERVAL &&
                    c->recache_counter++ % settings.ext_recache_rate == 0) {
                do_free = false;
                // In case it's been updated.
                it->exptime = h_it->exptime;
                it->it_flags &= ~ITEM_LINKED;
                it->refcount = 0;
                it->h_next = NULL; // might not be necessary.
                STORAGE_delete(c->thread->storage, h_it);
                item_replace(h_it, it, hv);
                pthread_mutex_lock(&c->thread->stats.mutex);
                c->thread->stats.recache_from_extstore++;
                pthread_mutex_unlock(&c->thread->stats.mutex);
            }
        }
        if (hold_lock)
            item_trylock_unlock(hold_lock);
    }
    if (do_free)
        slabs_free(it, ITEM_ntotal(it), ITEM_clsid(it));

    wrap->io.buf = NULL; // sanity.
    wrap->io.next = NULL;
    wrap->next = NULL;
    wrap->active = false;

    // TODO: reuse lock and/or hv.
    item_remove(wrap->hdr_it);
}