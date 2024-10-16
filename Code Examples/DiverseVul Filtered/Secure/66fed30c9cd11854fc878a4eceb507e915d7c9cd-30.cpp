static void bdrv_mirror_top_child_perm(BlockDriverState *bs, BdrvChild *c,
                                       BdrvChildRole role,
                                       BlockReopenQueue *reopen_queue,
                                       uint64_t perm, uint64_t shared,
                                       uint64_t *nperm, uint64_t *nshared)
{
    MirrorBDSOpaque *s = bs->opaque;

    if (s->stop) {
        /*
         * If the job is to be stopped, we do not need to forward
         * anything to the real image.
         */
        *nperm = 0;
        *nshared = BLK_PERM_ALL;
        return;
    }

    bdrv_default_perms(bs, c, role, reopen_queue,
                       perm, shared, nperm, nshared);

    if (s->is_commit) {
        /*
         * For commit jobs, we cannot take CONSISTENT_READ, because
         * that permission is unshared for everything above the base
         * node (except for filters on the base node).
         * We also have to force-share the WRITE permission, or
         * otherwise we would block ourselves at the base node (if
         * writes are blocked for a node, they are also blocked for
         * its backing file).
         * (We could also share RESIZE, because it may be needed for
         * the target if its size is less than the top node's; but
         * bdrv_default_perms_for_cow() automatically shares RESIZE
         * for backing nodes if WRITE is shared, so there is no need
         * to do it here.)
         */
        *nperm &= ~BLK_PERM_CONSISTENT_READ;
        *nshared |= BLK_PERM_WRITE;
    }
}