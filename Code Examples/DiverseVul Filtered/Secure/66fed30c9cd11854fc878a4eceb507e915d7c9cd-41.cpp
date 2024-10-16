static BlockJob *mirror_start_job(
                             const char *job_id, BlockDriverState *bs,
                             int creation_flags, BlockDriverState *target,
                             const char *replaces, int64_t speed,
                             uint32_t granularity, int64_t buf_size,
                             BlockMirrorBackingMode backing_mode,
                             bool zero_target,
                             BlockdevOnError on_source_error,
                             BlockdevOnError on_target_error,
                             bool unmap,
                             BlockCompletionFunc *cb,
                             void *opaque,
                             const BlockJobDriver *driver,
                             bool is_none_mode, BlockDriverState *base,
                             bool auto_complete, const char *filter_node_name,
                             bool is_mirror, MirrorCopyMode copy_mode,
                             Error **errp)
{
    MirrorBlockJob *s;
    MirrorBDSOpaque *bs_opaque;
    BlockDriverState *mirror_top_bs;
    bool target_is_backing;
    uint64_t target_perms, target_shared_perms;
    int ret;

    if (granularity == 0) {
        granularity = bdrv_get_default_bitmap_granularity(target);
    }

    assert(is_power_of_2(granularity));

    if (buf_size < 0) {
        error_setg(errp, "Invalid parameter 'buf-size'");
        return NULL;
    }

    if (buf_size == 0) {
        buf_size = DEFAULT_MIRROR_BUF_SIZE;
    }

    if (bdrv_skip_filters(bs) == bdrv_skip_filters(target)) {
        error_setg(errp, "Can't mirror node into itself");
        return NULL;
    }

    target_is_backing = bdrv_chain_contains(bs, target);

    /* In the case of active commit, add dummy driver to provide consistent
     * reads on the top, while disabling it in the intermediate nodes, and make
     * the backing chain writable. */
    mirror_top_bs = bdrv_new_open_driver(&bdrv_mirror_top, filter_node_name,
                                         BDRV_O_RDWR, errp);
    if (mirror_top_bs == NULL) {
        return NULL;
    }
    if (!filter_node_name) {
        mirror_top_bs->implicit = true;
    }

    /* So that we can always drop this node */
    mirror_top_bs->never_freeze = true;

    mirror_top_bs->total_sectors = bs->total_sectors;
    mirror_top_bs->supported_write_flags = BDRV_REQ_WRITE_UNCHANGED;
    mirror_top_bs->supported_zero_flags = BDRV_REQ_WRITE_UNCHANGED |
                                          BDRV_REQ_NO_FALLBACK;
    bs_opaque = g_new0(MirrorBDSOpaque, 1);
    mirror_top_bs->opaque = bs_opaque;

    bs_opaque->is_commit = target_is_backing;

    bdrv_drained_begin(bs);
    ret = bdrv_append(mirror_top_bs, bs, errp);
    bdrv_drained_end(bs);

    if (ret < 0) {
        bdrv_unref(mirror_top_bs);
        return NULL;
    }

    /* Make sure that the source is not resized while the job is running */
    s = block_job_create(job_id, driver, NULL, mirror_top_bs,
                         BLK_PERM_CONSISTENT_READ,
                         BLK_PERM_CONSISTENT_READ | BLK_PERM_WRITE_UNCHANGED |
                         BLK_PERM_WRITE | BLK_PERM_GRAPH_MOD, speed,
                         creation_flags, cb, opaque, errp);
    if (!s) {
        goto fail;
    }
    bs_opaque->job = s;

    /* The block job now has a reference to this node */
    bdrv_unref(mirror_top_bs);

    s->mirror_top_bs = mirror_top_bs;

    /* No resize for the target either; while the mirror is still running, a
     * consistent read isn't necessarily possible. We could possibly allow
     * writes and graph modifications, though it would likely defeat the
     * purpose of a mirror, so leave them blocked for now.
     *
     * In the case of active commit, things look a bit different, though,
     * because the target is an already populated backing file in active use.
     * We can allow anything except resize there.*/

    target_perms = BLK_PERM_WRITE;
    target_shared_perms = BLK_PERM_WRITE_UNCHANGED;

    if (target_is_backing) {
        int64_t bs_size, target_size;
        bs_size = bdrv_getlength(bs);
        if (bs_size < 0) {
            error_setg_errno(errp, -bs_size,
                             "Could not inquire top image size");
            goto fail;
        }

        target_size = bdrv_getlength(target);
        if (target_size < 0) {
            error_setg_errno(errp, -target_size,
                             "Could not inquire base image size");
            goto fail;
        }

        if (target_size < bs_size) {
            target_perms |= BLK_PERM_RESIZE;
        }

        target_shared_perms |= BLK_PERM_CONSISTENT_READ
                            |  BLK_PERM_WRITE
                            |  BLK_PERM_GRAPH_MOD;
    } else if (bdrv_chain_contains(bs, bdrv_skip_filters(target))) {
        /*
         * We may want to allow this in the future, but it would
         * require taking some extra care.
         */
        error_setg(errp, "Cannot mirror to a filter on top of a node in the "
                   "source's backing chain");
        goto fail;
    }

    if (backing_mode != MIRROR_LEAVE_BACKING_CHAIN) {
        target_perms |= BLK_PERM_GRAPH_MOD;
    }

    s->target = blk_new(s->common.job.aio_context,
                        target_perms, target_shared_perms);
    ret = blk_insert_bs(s->target, target, errp);
    if (ret < 0) {
        goto fail;
    }
    if (is_mirror) {
        /* XXX: Mirror target could be a NBD server of target QEMU in the case
         * of non-shared block migration. To allow migration completion, we
         * have to allow "inactivate" of the target BB.  When that happens, we
         * know the job is drained, and the vcpus are stopped, so no write
         * operation will be performed. Block layer already has assertions to
         * ensure that. */
        blk_set_force_allow_inactivate(s->target);
    }
    blk_set_allow_aio_context_change(s->target, true);
    blk_set_disable_request_queuing(s->target, true);

    s->replaces = g_strdup(replaces);
    s->on_source_error = on_source_error;
    s->on_target_error = on_target_error;
    s->is_none_mode = is_none_mode;
    s->backing_mode = backing_mode;
    s->zero_target = zero_target;
    s->copy_mode = copy_mode;
    s->base = base;
    s->base_overlay = bdrv_find_overlay(bs, base);
    s->granularity = granularity;
    s->buf_size = ROUND_UP(buf_size, granularity);
    s->unmap = unmap;
    if (auto_complete) {
        s->should_complete = true;
    }

    s->dirty_bitmap = bdrv_create_dirty_bitmap(bs, granularity, NULL, errp);
    if (!s->dirty_bitmap) {
        goto fail;
    }
    if (s->copy_mode == MIRROR_COPY_MODE_WRITE_BLOCKING) {
        bdrv_disable_dirty_bitmap(s->dirty_bitmap);
    }

    ret = block_job_add_bdrv(&s->common, "source", bs, 0,
                             BLK_PERM_WRITE_UNCHANGED | BLK_PERM_WRITE |
                             BLK_PERM_CONSISTENT_READ,
                             errp);
    if (ret < 0) {
        goto fail;
    }

    /* Required permissions are already taken with blk_new() */
    block_job_add_bdrv(&s->common, "target", target, 0, BLK_PERM_ALL,
                       &error_abort);

    /* In commit_active_start() all intermediate nodes disappear, so
     * any jobs in them must be blocked */
    if (target_is_backing) {
        BlockDriverState *iter, *filtered_target;
        uint64_t iter_shared_perms;

        /*
         * The topmost node with
         * bdrv_skip_filters(filtered_target) == bdrv_skip_filters(target)
         */
        filtered_target = bdrv_cow_bs(bdrv_find_overlay(bs, target));

        assert(bdrv_skip_filters(filtered_target) ==
               bdrv_skip_filters(target));

        /*
         * XXX BLK_PERM_WRITE needs to be allowed so we don't block
         * ourselves at s->base (if writes are blocked for a node, they are
         * also blocked for its backing file). The other options would be a
         * second filter driver above s->base (== target).
         */
        iter_shared_perms = BLK_PERM_WRITE_UNCHANGED | BLK_PERM_WRITE;

        for (iter = bdrv_filter_or_cow_bs(bs); iter != target;
             iter = bdrv_filter_or_cow_bs(iter))
        {
            if (iter == filtered_target) {
                /*
                 * From here on, all nodes are filters on the base.
                 * This allows us to share BLK_PERM_CONSISTENT_READ.
                 */
                iter_shared_perms |= BLK_PERM_CONSISTENT_READ;
            }

            ret = block_job_add_bdrv(&s->common, "intermediate node", iter, 0,
                                     iter_shared_perms, errp);
            if (ret < 0) {
                goto fail;
            }
        }

        if (bdrv_freeze_backing_chain(mirror_top_bs, target, errp) < 0) {
            goto fail;
        }
    }

    QTAILQ_INIT(&s->ops_in_flight);

    trace_mirror_start(bs, s, opaque);
    job_start(&s->common.job);

    return &s->common;

fail:
    if (s) {
        /* Make sure this BDS does not go away until we have completed the graph
         * changes below */
        bdrv_ref(mirror_top_bs);

        g_free(s->replaces);
        blk_unref(s->target);
        bs_opaque->job = NULL;
        if (s->dirty_bitmap) {
            bdrv_release_dirty_bitmap(s->dirty_bitmap);
        }
        job_early_fail(&s->common.job);
    }

    bs_opaque->stop = true;
    bdrv_child_refresh_perms(mirror_top_bs, mirror_top_bs->backing,
                             &error_abort);
    bdrv_replace_node(mirror_top_bs, mirror_top_bs->backing->bs, &error_abort);

    bdrv_unref(mirror_top_bs);

    return NULL;
}