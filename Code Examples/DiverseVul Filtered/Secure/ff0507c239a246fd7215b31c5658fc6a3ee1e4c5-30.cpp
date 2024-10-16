static void iscsi_retry_timer_expired(void *opaque)
{
    struct IscsiTask *iTask = opaque;
    iTask->complete = 1;
    if (iTask->co) {
        aio_co_wake(iTask->co);
    }
}