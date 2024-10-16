static void hid_del_idle_timer(HIDState *hs)
{
    if (hs->idle_timer) {
        timer_del(hs->idle_timer);
        timer_free(hs->idle_timer);
        hs->idle_timer = NULL;
    }
}