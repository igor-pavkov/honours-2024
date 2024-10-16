static void iscsi_close(BlockDriverState *bs)
{
    IscsiLun *iscsilun = bs->opaque;
    struct iscsi_context *iscsi = iscsilun->iscsi;

    iscsi_detach_aio_context(bs);
    if (iscsi_is_logged_in(iscsi)) {
        iscsi_logout_sync(iscsi);
    }
    iscsi_destroy_context(iscsi);
    if (iscsilun->dd) {
        g_free(iscsilun->dd->designator);
        g_free(iscsilun->dd);
    }
    g_free(iscsilun->zeroblock);
    iscsi_allocmap_free(iscsilun);
    qemu_mutex_destroy(&iscsilun->mutex);
    memset(iscsilun, 0, sizeof(IscsiLun));
}