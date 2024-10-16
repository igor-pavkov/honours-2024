static int coroutine_fn mirror_dirty_init(MirrorBlockJob *s)
{
    int64_t offset;
    BlockDriverState *bs = s->mirror_top_bs->backing->bs;
    BlockDriverState *target_bs = blk_bs(s->target);
    int ret;
    int64_t count;

    if (s->zero_target) {
        if (!bdrv_can_write_zeroes_with_unmap(target_bs)) {
            bdrv_set_dirty_bitmap(s->dirty_bitmap, 0, s->bdev_length);
            return 0;
        }

        s->initial_zeroing_ongoing = true;
        for (offset = 0; offset < s->bdev_length; ) {
            int bytes = MIN(s->bdev_length - offset,
                            QEMU_ALIGN_DOWN(INT_MAX, s->granularity));

            mirror_throttle(s);

            if (job_is_cancelled(&s->common.job)) {
                s->initial_zeroing_ongoing = false;
                return 0;
            }

            if (s->in_flight >= MAX_IN_FLIGHT) {
                trace_mirror_yield(s, UINT64_MAX, s->buf_free_count,
                                   s->in_flight);
                mirror_wait_for_free_in_flight_slot(s);
                continue;
            }

            mirror_perform(s, offset, bytes, MIRROR_METHOD_ZERO);
            offset += bytes;
        }

        mirror_wait_for_all_io(s);
        s->initial_zeroing_ongoing = false;
    }

    /* First part, loop on the sectors and initialize the dirty bitmap.  */
    for (offset = 0; offset < s->bdev_length; ) {
        /* Just to make sure we are not exceeding int limit. */
        int bytes = MIN(s->bdev_length - offset,
                        QEMU_ALIGN_DOWN(INT_MAX, s->granularity));

        mirror_throttle(s);

        if (job_is_cancelled(&s->common.job)) {
            return 0;
        }

        ret = bdrv_is_allocated_above(bs, s->base_overlay, true, offset, bytes,
                                      &count);
        if (ret < 0) {
            return ret;
        }

        assert(count);
        if (ret > 0) {
            bdrv_set_dirty_bitmap(s->dirty_bitmap, offset, count);
        }
        offset += count;
    }
    return 0;
}