static int mirror_exit_common(Job *job)
{
    MirrorBlockJob *s = container_of(job, MirrorBlockJob, common.job);
    BlockJob *bjob = &s->common;
    MirrorBDSOpaque *bs_opaque;
    AioContext *replace_aio_context = NULL;
    BlockDriverState *src;
    BlockDriverState *target_bs;
    BlockDriverState *mirror_top_bs;
    Error *local_err = NULL;
    bool abort = job->ret < 0;
    int ret = 0;

    if (s->prepared) {
        return 0;
    }
    s->prepared = true;

    mirror_top_bs = s->mirror_top_bs;
    bs_opaque = mirror_top_bs->opaque;
    src = mirror_top_bs->backing->bs;
    target_bs = blk_bs(s->target);

    if (bdrv_chain_contains(src, target_bs)) {
        bdrv_unfreeze_backing_chain(mirror_top_bs, target_bs);
    }

    bdrv_release_dirty_bitmap(s->dirty_bitmap);

    /* Make sure that the source BDS doesn't go away during bdrv_replace_node,
     * before we can call bdrv_drained_end */
    bdrv_ref(src);
    bdrv_ref(mirror_top_bs);
    bdrv_ref(target_bs);

    /*
     * Remove target parent that still uses BLK_PERM_WRITE/RESIZE before
     * inserting target_bs at s->to_replace, where we might not be able to get
     * these permissions.
     */
    blk_unref(s->target);
    s->target = NULL;

    /* We don't access the source any more. Dropping any WRITE/RESIZE is
     * required before it could become a backing file of target_bs. Not having
     * these permissions any more means that we can't allow any new requests on
     * mirror_top_bs from now on, so keep it drained. */
    bdrv_drained_begin(mirror_top_bs);
    bs_opaque->stop = true;
    bdrv_child_refresh_perms(mirror_top_bs, mirror_top_bs->backing,
                             &error_abort);
    if (!abort && s->backing_mode == MIRROR_SOURCE_BACKING_CHAIN) {
        BlockDriverState *backing = s->is_none_mode ? src : s->base;
        BlockDriverState *unfiltered_target = bdrv_skip_filters(target_bs);

        if (bdrv_cow_bs(unfiltered_target) != backing) {
            bdrv_set_backing_hd(unfiltered_target, backing, &local_err);
            if (local_err) {
                error_report_err(local_err);
                local_err = NULL;
                ret = -EPERM;
            }
        }
    } else if (!abort && s->backing_mode == MIRROR_OPEN_BACKING_CHAIN) {
        assert(!bdrv_backing_chain_next(target_bs));
        ret = bdrv_open_backing_file(bdrv_skip_filters(target_bs), NULL,
                                     "backing", &local_err);
        if (ret < 0) {
            error_report_err(local_err);
            local_err = NULL;
        }
    }

    if (s->to_replace) {
        replace_aio_context = bdrv_get_aio_context(s->to_replace);
        aio_context_acquire(replace_aio_context);
    }

    if (s->should_complete && !abort) {
        BlockDriverState *to_replace = s->to_replace ?: src;
        bool ro = bdrv_is_read_only(to_replace);

        if (ro != bdrv_is_read_only(target_bs)) {
            bdrv_reopen_set_read_only(target_bs, ro, NULL);
        }

        /* The mirror job has no requests in flight any more, but we need to
         * drain potential other users of the BDS before changing the graph. */
        assert(s->in_drain);
        bdrv_drained_begin(target_bs);
        /*
         * Cannot use check_to_replace_node() here, because that would
         * check for an op blocker on @to_replace, and we have our own
         * there.
         */
        if (bdrv_recurse_can_replace(src, to_replace)) {
            bdrv_replace_node(to_replace, target_bs, &local_err);
        } else {
            error_setg(&local_err, "Can no longer replace '%s' by '%s', "
                       "because it can no longer be guaranteed that doing so "
                       "would not lead to an abrupt change of visible data",
                       to_replace->node_name, target_bs->node_name);
        }
        bdrv_drained_end(target_bs);
        if (local_err) {
            error_report_err(local_err);
            ret = -EPERM;
        }
    }
    if (s->to_replace) {
        bdrv_op_unblock_all(s->to_replace, s->replace_blocker);
        error_free(s->replace_blocker);
        bdrv_unref(s->to_replace);
    }
    if (replace_aio_context) {
        aio_context_release(replace_aio_context);
    }
    g_free(s->replaces);
    bdrv_unref(target_bs);

    /*
     * Remove the mirror filter driver from the graph. Before this, get rid of
     * the blockers on the intermediate nodes so that the resulting state is
     * valid.
     */
    block_job_remove_all_bdrv(bjob);
    bdrv_replace_node(mirror_top_bs, mirror_top_bs->backing->bs, &error_abort);

    /* We just changed the BDS the job BB refers to (with either or both of the
     * bdrv_replace_node() calls), so switch the BB back so the cleanup does
     * the right thing. We don't need any permissions any more now. */
    blk_remove_bs(bjob->blk);
    blk_set_perm(bjob->blk, 0, BLK_PERM_ALL, &error_abort);
    blk_insert_bs(bjob->blk, mirror_top_bs, &error_abort);

    bs_opaque->job = NULL;

    bdrv_drained_end(src);
    bdrv_drained_end(mirror_top_bs);
    s->in_drain = false;
    bdrv_unref(mirror_top_bs);
    bdrv_unref(src);

    return ret;
}