static int handle_dependencies(BlockDriverState *bs, uint64_t guest_offset,
    uint64_t *cur_bytes, QCowL2Meta **m)
{
    BDRVQcowState *s = bs->opaque;
    QCowL2Meta *old_alloc;
    uint64_t bytes = *cur_bytes;

    QLIST_FOREACH(old_alloc, &s->cluster_allocs, next_in_flight) {

        uint64_t start = guest_offset;
        uint64_t end = start + bytes;
        uint64_t old_start = l2meta_cow_start(old_alloc);
        uint64_t old_end = l2meta_cow_end(old_alloc);

        if (end <= old_start || start >= old_end) {
            /* No intersection */
        } else {
            if (start < old_start) {
                /* Stop at the start of a running allocation */
                bytes = old_start - start;
            } else {
                bytes = 0;
            }

            /* Stop if already an l2meta exists. After yielding, it wouldn't
             * be valid any more, so we'd have to clean up the old L2Metas
             * and deal with requests depending on them before starting to
             * gather new ones. Not worth the trouble. */
            if (bytes == 0 && *m) {
                *cur_bytes = 0;
                return 0;
            }

            if (bytes == 0) {
                /* Wait for the dependency to complete. We need to recheck
                 * the free/allocated clusters when we continue. */
                qemu_co_mutex_unlock(&s->lock);
                qemu_co_queue_wait(&old_alloc->dependent_requests);
                qemu_co_mutex_lock(&s->lock);
                return -EAGAIN;
            }
        }
    }

    /* Make sure that existing clusters and new allocations are only used up to
     * the next dependency if we shortened the request above */
    *cur_bytes = bytes;

    return 0;
}