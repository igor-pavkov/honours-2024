static void iscsi_attach_aio_context(BlockDriverState *bs,
                                     AioContext *new_context)
{
    IscsiLun *iscsilun = bs->opaque;

    iscsilun->aio_context = new_context;
    iscsi_set_events(iscsilun);

    /* Set up a timer for sending out iSCSI NOPs */
    iscsilun->nop_timer = aio_timer_new(iscsilun->aio_context,
                                        QEMU_CLOCK_REALTIME, SCALE_MS,
                                        iscsi_nop_timed_event, iscsilun);
    timer_mod(iscsilun->nop_timer,
              qemu_clock_get_ms(QEMU_CLOCK_REALTIME) + NOP_INTERVAL);

    /* Set up a timer for periodic calls to iscsi_set_events and to
     * scan for command timeout */
    iscsilun->event_timer = aio_timer_new(iscsilun->aio_context,
                                          QEMU_CLOCK_REALTIME, SCALE_MS,
                                          iscsi_timed_check_events, iscsilun);
    timer_mod(iscsilun->event_timer,
              qemu_clock_get_ms(QEMU_CLOCK_REALTIME) + EVENT_INTERVAL);
}