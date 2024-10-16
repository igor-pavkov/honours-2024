static void bdrv_dev_change_media_cb(BlockDriverState *bs, bool load)
{
    if (bs->dev_ops && bs->dev_ops->change_media_cb) {
        bool tray_was_closed = !bdrv_dev_is_tray_open(bs);
        bs->dev_ops->change_media_cb(bs->dev_opaque, load);
        if (tray_was_closed) {
            /* tray open */
            bdrv_emit_qmp_eject_event(bs, true);
        }
        if (load) {
            /* tray close */
            bdrv_emit_qmp_eject_event(bs, false);
        }
    }
}