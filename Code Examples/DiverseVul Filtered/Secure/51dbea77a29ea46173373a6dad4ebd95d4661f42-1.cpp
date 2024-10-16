void hid_set_next_idle(HIDState *hs)
{
    if (hs->idle) {
        uint64_t expire_time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) +
                               NANOSECONDS_PER_SECOND * hs->idle * 4 / 1000;
        if (!hs->idle_timer) {
            hs->idle_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, hid_idle_timer, hs);
        }
        timer_mod_ns(hs->idle_timer, expire_time);
    } else {
        hid_del_idle_timer(hs);
    }
}