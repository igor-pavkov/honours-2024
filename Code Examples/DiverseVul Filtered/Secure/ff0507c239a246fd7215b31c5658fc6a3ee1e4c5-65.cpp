static void iscsi_co_generic_bh_cb(void *opaque)
{
    struct IscsiTask *iTask = opaque;

    iTask->complete = 1;
    aio_co_wake(iTask->co);
}