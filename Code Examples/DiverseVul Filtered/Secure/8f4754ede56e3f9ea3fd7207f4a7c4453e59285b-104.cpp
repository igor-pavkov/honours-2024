void bdrv_eject(BlockDriverState *bs, bool eject_flag)
{
    BlockDriver *drv = bs->drv;

    if (drv && drv->bdrv_eject) {
        drv->bdrv_eject(bs, eject_flag);
    }

    if (bs->device_name[0] != '\0') {
        bdrv_emit_qmp_eject_event(bs, eject_flag);
    }
}