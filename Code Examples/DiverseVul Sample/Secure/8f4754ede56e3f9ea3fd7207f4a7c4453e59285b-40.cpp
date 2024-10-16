void qemu_aio_release(void *p)
{
    BlockDriverAIOCB *acb = p;
    g_slice_free1(acb->aiocb_info->aiocb_size, acb);
}