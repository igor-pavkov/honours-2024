static int coroutine_fn mirror_run(Job *job, Error **errp)
{
    MirrorBlockJob *s = container_of(job, MirrorBlockJob, common.job);
    BlockDriverState *bs = s->mirror_top_bs->backing->bs;
    BlockDriverState *target_bs = blk_bs(s->target);
    bool need_drain = true;
    int64_t length;
    int64_t target_length;
    BlockDriverInfo bdi;
    char backing_filename[2]; /* we only need 2 characters because we are only
                                 checking for a NULL string */
    int ret = 0;

    if (job_is_cancelled(&s->common.job)) {
        goto immediate_exit;
    }

    s->bdev_length = bdrv_getlength(bs);
    if (s->bdev_length < 0) {
        ret = s->bdev_length;
        goto immediate_exit;
    }

    target_length = blk_getlength(s->target);
    if (target_length < 0) {
        ret = target_length;
        goto immediate_exit;
    }

    /* Active commit must resize the base image if its size differs from the
     * active layer. */
    if (s->base == blk_bs(s->target)) {
        if (s->bdev_length > target_length) {
            ret = blk_truncate(s->target, s->bdev_length, false,
                               PREALLOC_MODE_OFF, 0, NULL);
            if (ret < 0) {
                goto immediate_exit;
            }
        }
    } else if (s->bdev_length != target_length) {
        error_setg(errp, "Source and target image have different sizes");
        ret = -EINVAL;
        goto immediate_exit;
    }

    if (s->bdev_length == 0) {
        /* Transition to the READY state and wait for complete. */
        job_transition_to_ready(&s->common.job);
        s->synced = true;
        s->actively_synced = true;
        while (!job_is_cancelled(&s->common.job) && !s->should_complete) {
            job_yield(&s->common.job);
        }
        s->common.job.cancelled = false;
        goto immediate_exit;
    }

    length = DIV_ROUND_UP(s->bdev_length, s->granularity);
    s->in_flight_bitmap = bitmap_new(length);

    /* If we have no backing file yet in the destination, we cannot let
     * the destination do COW.  Instead, we copy sectors around the
     * dirty data if needed.  We need a bitmap to do that.
     */
    bdrv_get_backing_filename(target_bs, backing_filename,
                              sizeof(backing_filename));
    if (!bdrv_get_info(target_bs, &bdi) && bdi.cluster_size) {
        s->target_cluster_size = bdi.cluster_size;
    } else {
        s->target_cluster_size = BDRV_SECTOR_SIZE;
    }
    if (backing_filename[0] && !bdrv_backing_chain_next(target_bs) &&
        s->granularity < s->target_cluster_size) {
        s->buf_size = MAX(s->buf_size, s->target_cluster_size);
        s->cow_bitmap = bitmap_new(length);
    }
    s->max_iov = MIN(bs->bl.max_iov, target_bs->bl.max_iov);

    s->buf = qemu_try_blockalign(bs, s->buf_size);
    if (s->buf == NULL) {
        ret = -ENOMEM;
        goto immediate_exit;
    }

    mirror_free_init(s);

    s->last_pause_ns = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);
    if (!s->is_none_mode) {
        ret = mirror_dirty_init(s);
        if (ret < 0 || job_is_cancelled(&s->common.job)) {
            goto immediate_exit;
        }
    }

    assert(!s->dbi);
    s->dbi = bdrv_dirty_iter_new(s->dirty_bitmap);
    for (;;) {
        uint64_t delay_ns = 0;
        int64_t cnt, delta;
        bool should_complete;

        /* Do not start passive operations while there are active
         * writes in progress */
        while (s->in_active_write_counter) {
            mirror_wait_for_any_operation(s, true);
        }

        if (s->ret < 0) {
            ret = s->ret;
            goto immediate_exit;
        }

        job_pause_point(&s->common.job);

        cnt = bdrv_get_dirty_count(s->dirty_bitmap);
        /* cnt is the number of dirty bytes remaining and s->bytes_in_flight is
         * the number of bytes currently being processed; together those are
         * the current remaining operation length */
        job_progress_set_remaining(&s->common.job, s->bytes_in_flight + cnt);

        /* Note that even when no rate limit is applied we need to yield
         * periodically with no pending I/O so that bdrv_drain_all() returns.
         * We do so every BLKOCK_JOB_SLICE_TIME nanoseconds, or when there is
         * an error, or when the source is clean, whichever comes first. */
        delta = qemu_clock_get_ns(QEMU_CLOCK_REALTIME) - s->last_pause_ns;
        if (delta < BLOCK_JOB_SLICE_TIME &&
            s->common.iostatus == BLOCK_DEVICE_IO_STATUS_OK) {
            if (s->in_flight >= MAX_IN_FLIGHT || s->buf_free_count == 0 ||
                (cnt == 0 && s->in_flight > 0)) {
                trace_mirror_yield(s, cnt, s->buf_free_count, s->in_flight);
                mirror_wait_for_free_in_flight_slot(s);
                continue;
            } else if (cnt != 0) {
                delay_ns = mirror_iteration(s);
            }
        }

        should_complete = false;
        if (s->in_flight == 0 && cnt == 0) {
            trace_mirror_before_flush(s);
            if (!s->synced) {
                if (mirror_flush(s) < 0) {
                    /* Go check s->ret.  */
                    continue;
                }
                /* We're out of the streaming phase.  From now on, if the job
                 * is cancelled we will actually complete all pending I/O and
                 * report completion.  This way, block-job-cancel will leave
                 * the target in a consistent state.
                 */
                job_transition_to_ready(&s->common.job);
                s->synced = true;
                if (s->copy_mode != MIRROR_COPY_MODE_BACKGROUND) {
                    s->actively_synced = true;
                }
            }

            should_complete = s->should_complete ||
                job_is_cancelled(&s->common.job);
            cnt = bdrv_get_dirty_count(s->dirty_bitmap);
        }

        if (cnt == 0 && should_complete) {
            /* The dirty bitmap is not updated while operations are pending.
             * If we're about to exit, wait for pending operations before
             * calling bdrv_get_dirty_count(bs), or we may exit while the
             * source has dirty data to copy!
             *
             * Note that I/O can be submitted by the guest while
             * mirror_populate runs, so pause it now.  Before deciding
             * whether to switch to target check one last time if I/O has
             * come in the meanwhile, and if not flush the data to disk.
             */
            trace_mirror_before_drain(s, cnt);

            s->in_drain = true;
            bdrv_drained_begin(bs);
            cnt = bdrv_get_dirty_count(s->dirty_bitmap);
            if (cnt > 0 || mirror_flush(s) < 0) {
                bdrv_drained_end(bs);
                s->in_drain = false;
                continue;
            }

            /* The two disks are in sync.  Exit and report successful
             * completion.
             */
            assert(QLIST_EMPTY(&bs->tracked_requests));
            s->common.job.cancelled = false;
            need_drain = false;
            break;
        }

        ret = 0;

        if (s->synced && !should_complete) {
            delay_ns = (s->in_flight == 0 &&
                        cnt == 0 ? BLOCK_JOB_SLICE_TIME : 0);
        }
        trace_mirror_before_sleep(s, cnt, s->synced, delay_ns);
        job_sleep_ns(&s->common.job, delay_ns);
        if (job_is_cancelled(&s->common.job) &&
            (!s->synced || s->common.job.force_cancel))
        {
            break;
        }
        s->last_pause_ns = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);
    }

immediate_exit:
    if (s->in_flight > 0) {
        /* We get here only if something went wrong.  Either the job failed,
         * or it was cancelled prematurely so that we do not guarantee that
         * the target is a copy of the source.
         */
        assert(ret < 0 || ((s->common.job.force_cancel || !s->synced) &&
               job_is_cancelled(&s->common.job)));
        assert(need_drain);
        mirror_wait_for_all_io(s);
    }

    assert(s->in_flight == 0);
    qemu_vfree(s->buf);
    g_free(s->cow_bitmap);
    g_free(s->in_flight_bitmap);
    bdrv_dirty_iter_free(s->dbi);

    if (need_drain) {
        s->in_drain = true;
        bdrv_drained_begin(bs);
    }

    return ret;
}