static void bdrv_co_em_bh(void *opaque)
{
    BlockDriverAIOCBCoroutine *acb = opaque;

    acb->common.cb(acb->common.opaque, acb->req.error);

    if (acb->done) {
        *acb->done = true;
    }

    qemu_bh_delete(acb->bh);
    qemu_aio_release(acb);
}