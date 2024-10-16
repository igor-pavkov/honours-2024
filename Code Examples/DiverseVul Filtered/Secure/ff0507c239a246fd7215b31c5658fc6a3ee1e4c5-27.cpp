static void coroutine_fn iscsi_co_wait_for_task(IscsiTask *iTask,
                                                IscsiLun *iscsilun)
{
    while (!iTask->complete) {
        iscsi_set_events(iscsilun);
        qemu_mutex_unlock(&iscsilun->mutex);
        qemu_coroutine_yield();
        qemu_mutex_lock(&iscsilun->mutex);
    }
}