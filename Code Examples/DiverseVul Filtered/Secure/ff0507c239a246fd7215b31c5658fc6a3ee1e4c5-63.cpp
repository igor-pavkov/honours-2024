static void iscsi_ioctl_handle_emulated(IscsiAIOCB *acb, int req, void *buf)
{
    BlockDriverState *bs = acb->common.bs;
    IscsiLun *iscsilun = bs->opaque;
    int ret = 0;

    switch (req) {
    case SG_GET_VERSION_NUM:
        *(int *)buf = 30000;
        break;
    case SG_GET_SCSI_ID:
        ((struct sg_scsi_id *)buf)->scsi_type = iscsilun->type;
        break;
    default:
        ret = -EINVAL;
    }
    assert(!acb->bh);
    acb->bh = aio_bh_new(bdrv_get_aio_context(bs),
                         iscsi_ioctl_bh_completion, acb);
    acb->ret = ret;
    qemu_bh_schedule(acb->bh);
}