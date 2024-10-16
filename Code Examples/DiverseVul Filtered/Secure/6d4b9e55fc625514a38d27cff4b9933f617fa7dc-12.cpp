static int curl_timer_cb(CURLM *multi, long timeout_ms, void *opaque)
{
    BDRVCURLState *s = opaque;

    DPRINTF("CURL: timer callback timeout_ms %ld\n", timeout_ms);
    if (timeout_ms == -1) {
        timer_del(&s->timer);
    } else {
        int64_t timeout_ns = (int64_t)timeout_ms * 1000 * 1000;
        timer_mod(&s->timer,
                  qemu_clock_get_ns(QEMU_CLOCK_REALTIME) + timeout_ns);
    }
    return 0;
}