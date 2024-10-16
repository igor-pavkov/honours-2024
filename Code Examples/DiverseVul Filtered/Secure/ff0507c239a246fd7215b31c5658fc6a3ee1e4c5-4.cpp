static void iscsi_detach_aio_context(BlockDriverState *bs)
{
    IscsiLun *iscsilun = bs->opaque;

    aio_set_fd_handler(iscsilun->aio_context, iscsi_get_fd(iscsilun->iscsi),
                       false, NULL, NULL, NULL, NULL);
    iscsilun->events = 0;

    if (iscsilun->nop_timer) {
        timer_del(iscsilun->nop_timer);
        timer_free(iscsilun->nop_timer);
        iscsilun->nop_timer = NULL;
    }
    if (iscsilun->event_timer) {
        timer_del(iscsilun->event_timer);
        timer_free(iscsilun->event_timer);
        iscsilun->event_timer = NULL;
    }
}