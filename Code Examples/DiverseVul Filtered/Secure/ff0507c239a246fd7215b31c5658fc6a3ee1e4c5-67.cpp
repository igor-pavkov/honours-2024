static void iscsi_ioctl_bh_completion(void *opaque)
{
    IscsiAIOCB *acb = opaque;

    qemu_bh_delete(acb->bh);
    acb->common.cb(acb->common.opaque, acb->ret);
    qemu_aio_unref(acb);
}