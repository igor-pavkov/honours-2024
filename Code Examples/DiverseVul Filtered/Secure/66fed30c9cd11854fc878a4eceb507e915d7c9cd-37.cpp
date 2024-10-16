static uint64_t coroutine_fn mirror_iteration(MirrorBlockJob *s)
{
    BlockDriverState *source = s->mirror_top_bs->backing->bs;
    MirrorOp *pseudo_op;
    int64_t offset;
    uint64_t delay_ns = 0, ret = 0;
    /* At least the first dirty chunk is mirrored in one iteration. */
    int nb_chunks = 1;
    bool write_zeroes_ok = bdrv_can_write_zeroes_with_unmap(blk_bs(s->target));
    int max_io_bytes = MAX(s->buf_size / MAX_IN_FLIGHT, MAX_IO_BYTES);

    bdrv_dirty_bitmap_lock(s->dirty_bitmap);
    offset = bdrv_dirty_iter_next(s->dbi);
    if (offset < 0) {
        bdrv_set_dirty_iter(s->dbi, 0);
        offset = bdrv_dirty_iter_next(s->dbi);
        trace_mirror_restart_iter(s, bdrv_get_dirty_count(s->dirty_bitmap));
        assert(offset >= 0);
    }
    bdrv_dirty_bitmap_unlock(s->dirty_bitmap);

    mirror_wait_on_conflicts(NULL, s, offset, 1);

    job_pause_point(&s->common.job);

    /* Find the number of consective dirty chunks following the first dirty
     * one, and wait for in flight requests in them. */
    bdrv_dirty_bitmap_lock(s->dirty_bitmap);
    while (nb_chunks * s->granularity < s->buf_size) {
        int64_t next_dirty;
        int64_t next_offset = offset + nb_chunks * s->granularity;
        int64_t next_chunk = next_offset / s->granularity;
        if (next_offset >= s->bdev_length ||
            !bdrv_dirty_bitmap_get_locked(s->dirty_bitmap, next_offset)) {
            break;
        }
        if (test_bit(next_chunk, s->in_flight_bitmap)) {
            break;
        }

        next_dirty = bdrv_dirty_iter_next(s->dbi);
        if (next_dirty > next_offset || next_dirty < 0) {
            /* The bitmap iterator's cache is stale, refresh it */
            bdrv_set_dirty_iter(s->dbi, next_offset);
            next_dirty = bdrv_dirty_iter_next(s->dbi);
        }
        assert(next_dirty == next_offset);
        nb_chunks++;
    }

    /* Clear dirty bits before querying the block status, because
     * calling bdrv_block_status_above could yield - if some blocks are
     * marked dirty in this window, we need to know.
     */
    bdrv_reset_dirty_bitmap_locked(s->dirty_bitmap, offset,
                                   nb_chunks * s->granularity);
    bdrv_dirty_bitmap_unlock(s->dirty_bitmap);

    /* Before claiming an area in the in-flight bitmap, we have to
     * create a MirrorOp for it so that conflicting requests can wait
     * for it.  mirror_perform() will create the real MirrorOps later,
     * for now we just create a pseudo operation that will wake up all
     * conflicting requests once all real operations have been
     * launched. */
    pseudo_op = g_new(MirrorOp, 1);
    *pseudo_op = (MirrorOp){
        .offset         = offset,
        .bytes          = nb_chunks * s->granularity,
        .is_pseudo_op   = true,
    };
    qemu_co_queue_init(&pseudo_op->waiting_requests);
    QTAILQ_INSERT_TAIL(&s->ops_in_flight, pseudo_op, next);

    bitmap_set(s->in_flight_bitmap, offset / s->granularity, nb_chunks);
    while (nb_chunks > 0 && offset < s->bdev_length) {
        int ret;
        int64_t io_bytes;
        int64_t io_bytes_acct;
        MirrorMethod mirror_method = MIRROR_METHOD_COPY;

        assert(!(offset % s->granularity));
        ret = bdrv_block_status_above(source, NULL, offset,
                                      nb_chunks * s->granularity,
                                      &io_bytes, NULL, NULL);
        if (ret < 0) {
            io_bytes = MIN(nb_chunks * s->granularity, max_io_bytes);
        } else if (ret & BDRV_BLOCK_DATA) {
            io_bytes = MIN(io_bytes, max_io_bytes);
        }

        io_bytes -= io_bytes % s->granularity;
        if (io_bytes < s->granularity) {
            io_bytes = s->granularity;
        } else if (ret >= 0 && !(ret & BDRV_BLOCK_DATA)) {
            int64_t target_offset;
            int64_t target_bytes;
            bdrv_round_to_clusters(blk_bs(s->target), offset, io_bytes,
                                   &target_offset, &target_bytes);
            if (target_offset == offset &&
                target_bytes == io_bytes) {
                mirror_method = ret & BDRV_BLOCK_ZERO ?
                                    MIRROR_METHOD_ZERO :
                                    MIRROR_METHOD_DISCARD;
            }
        }

        while (s->in_flight >= MAX_IN_FLIGHT) {
            trace_mirror_yield_in_flight(s, offset, s->in_flight);
            mirror_wait_for_free_in_flight_slot(s);
        }

        if (s->ret < 0) {
            ret = 0;
            goto fail;
        }

        io_bytes = mirror_clip_bytes(s, offset, io_bytes);
        io_bytes = mirror_perform(s, offset, io_bytes, mirror_method);
        if (mirror_method != MIRROR_METHOD_COPY && write_zeroes_ok) {
            io_bytes_acct = 0;
        } else {
            io_bytes_acct = io_bytes;
        }
        assert(io_bytes);
        offset += io_bytes;
        nb_chunks -= DIV_ROUND_UP(io_bytes, s->granularity);
        delay_ns = block_job_ratelimit_get_delay(&s->common, io_bytes_acct);
    }

    ret = delay_ns;
fail:
    QTAILQ_REMOVE(&s->ops_in_flight, pseudo_op, next);
    qemu_co_queue_restart_all(&pseudo_op->waiting_requests);
    g_free(pseudo_op);

    return ret;
}