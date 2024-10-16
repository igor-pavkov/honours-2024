static void trim_aio_cancel(BlockDriverAIOCB *acb)
{
    TrimAIOCB *iocb = container_of(acb, TrimAIOCB, common);

    /* Exit the loop in case bdrv_aio_cancel calls ide_issue_trim_cb again.  */
    iocb->j = iocb->qiov->niov - 1;
    iocb->i = (iocb->qiov->iov[iocb->j].iov_len / 8) - 1;

    /* Tell ide_issue_trim_cb not to trigger the completion, too.  */
    qemu_bh_delete(iocb->bh);
    iocb->bh = NULL;

    if (iocb->aiocb) {
        bdrv_aio_cancel(iocb->aiocb);
    }
    qemu_aio_release(iocb);
}