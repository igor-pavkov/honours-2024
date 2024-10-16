static void coroutine_fn iscsi_co_invalidate_cache(BlockDriverState *bs,
                                                   Error **errp)
{
    IscsiLun *iscsilun = bs->opaque;
    iscsi_allocmap_invalidate(iscsilun);
}