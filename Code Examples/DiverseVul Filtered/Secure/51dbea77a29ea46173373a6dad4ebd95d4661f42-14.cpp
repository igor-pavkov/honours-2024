static void hid_idle_timer(void *opaque)
{
    HIDState *hs = opaque;

    hs->idle_pending = true;
    hs->event(hs);
}