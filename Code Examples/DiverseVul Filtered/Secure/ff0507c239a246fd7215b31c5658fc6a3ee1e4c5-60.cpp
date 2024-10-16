static void iscsi_timed_check_events(void *opaque)
{
    IscsiLun *iscsilun = opaque;

    qemu_mutex_lock(&iscsilun->mutex);

    /* check for timed out requests */
    iscsi_service(iscsilun->iscsi, 0);

    if (iscsilun->request_timed_out) {
        iscsilun->request_timed_out = false;
        iscsi_reconnect(iscsilun->iscsi);
    }

    /* newer versions of libiscsi may return zero events. Ensure we are able
     * to return to service once this situation changes. */
    iscsi_set_events(iscsilun);

    qemu_mutex_unlock(&iscsilun->mutex);

    timer_mod(iscsilun->event_timer,
              qemu_clock_get_ms(QEMU_CLOCK_REALTIME) + EVENT_INTERVAL);
}