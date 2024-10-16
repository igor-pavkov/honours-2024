void bdrv_aio_cancel(BlockDriverAIOCB *acb)
{
    acb->aiocb_info->cancel(acb);
}