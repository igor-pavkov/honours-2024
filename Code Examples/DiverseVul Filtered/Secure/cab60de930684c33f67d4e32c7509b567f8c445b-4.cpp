static int perform_cow(BlockDriverState *bs, QCowL2Meta *m, Qcow2COWRegion *r)
{
    BDRVQcowState *s = bs->opaque;
    int ret;

    if (r->nb_sectors == 0) {
        return 0;
    }

    qemu_co_mutex_unlock(&s->lock);
    ret = copy_sectors(bs, m->offset / BDRV_SECTOR_SIZE, m->alloc_offset,
                       r->offset / BDRV_SECTOR_SIZE,
                       r->offset / BDRV_SECTOR_SIZE + r->nb_sectors);
    qemu_co_mutex_lock(&s->lock);

    if (ret < 0) {
        return ret;
    }

    /*
     * Before we update the L2 table to actually point to the new cluster, we
     * need to be sure that the refcounts have been increased and COW was
     * handled.
     */
    qcow2_cache_depends_on_flush(s->l2_table_cache);

    return 0;
}