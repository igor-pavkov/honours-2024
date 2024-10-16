static inline int _get_extstore(conn *c, item *it, int iovst, int iovcnt) {
#ifdef NEED_ALIGN
    item_hdr hdr;
    memcpy(&hdr, ITEM_data(it), sizeof(hdr));
#else
    item_hdr *hdr = (item_hdr *)ITEM_data(it);
#endif
    size_t ntotal = ITEM_ntotal(it);
    unsigned int clsid = slabs_clsid(ntotal);
    item *new_it;
    bool chunked = false;
    if (ntotal > settings.slab_chunk_size_max) {
        // Pull a chunked item header.
        uint32_t flags;
        FLAGS_CONV(it, flags);
        new_it = item_alloc(ITEM_key(it), it->nkey, flags, it->exptime, it->nbytes);
        assert(new_it == NULL || (new_it->it_flags & ITEM_CHUNKED));
        chunked = true;
    } else {
        new_it = do_item_alloc_pull(ntotal, clsid);
    }
    if (new_it == NULL)
        return -1;
    assert(!c->io_queued); // FIXME: debugging.
    // so we can free the chunk on a miss
    new_it->slabs_clsid = clsid;

    io_wrap *io = do_cache_alloc(c->thread->io_cache);
    io->active = true;
    io->miss = false;
    io->badcrc = false;
    // io_wrap owns the reference for this object now.
    io->hdr_it = it;

    // FIXME: error handling.
    // The offsets we'll wipe on a miss.
    io->iovec_start = iovst;
    io->iovec_count = iovcnt;
    // This is probably super dangerous. keep it at 0 and fill into wrap
    // object?
    if (chunked) {
        unsigned int ciovcnt = 1;
        size_t remain = new_it->nbytes;
        item_chunk *chunk = (item_chunk *) ITEM_schunk(new_it);
        io->io.iov = &c->iov[c->iovused];
        // fill the header so we can get the full data + crc back.
        add_iov(c, new_it, ITEM_ntotal(new_it) - new_it->nbytes);
        while (remain > 0) {
            chunk = do_item_alloc_chunk(chunk, remain);
            if (chunk == NULL) {
                item_remove(new_it);
                do_cache_free(c->thread->io_cache, io);
                return -1;
            }
            add_iov(c, chunk->data, (remain < chunk->size) ? remain : chunk->size);
            chunk->used = (remain < chunk->size) ? remain : chunk->size;
            remain -= chunk->size;
            ciovcnt++;
        }
        io->io.iovcnt = ciovcnt;
        // header object was already accounted for, remove one from total
        io->iovec_count += ciovcnt-1;
    } else {
        io->io.iov = NULL;
        io->iovec_data = c->iovused;
        add_iov(c, "", it->nbytes);
    }
    io->io.buf = (void *)new_it;
    // The offset we'll fill in on a hit.
    io->c = c;
    // We need to stack the sub-struct IO's together as well.
    if (c->io_wraplist) {
        io->io.next = &c->io_wraplist->io;
    } else {
        io->io.next = NULL;
    }
    // IO queue for this connection.
    io->next = c->io_wraplist;
    c->io_wraplist = io;
    assert(c->io_wrapleft >= 0);
    c->io_wrapleft++;
    // reference ourselves for the callback.
    io->io.data = (void *)io;

    // Now, fill in io->io based on what was in our header.
#ifdef NEED_ALIGN
    io->io.page_version = hdr.page_version;
    io->io.page_id = hdr.page_id;
    io->io.offset = hdr.offset;
#else
    io->io.page_version = hdr->page_version;
    io->io.page_id = hdr->page_id;
    io->io.offset = hdr->offset;
#endif
    io->io.len = ntotal;
    io->io.mode = OBJ_IO_READ;
    io->io.cb = _get_extstore_cb;

    //fprintf(stderr, "EXTSTORE: IO stacked %u\n", io->iovec_data);
    // FIXME: This stat needs to move to reflect # of flash hits vs misses
    // for now it's a good gauge on how often we request out to flash at
    // least.
    pthread_mutex_lock(&c->thread->stats.mutex);
    c->thread->stats.get_extstore++;
    pthread_mutex_unlock(&c->thread->stats.mutex);

    return 0;
}