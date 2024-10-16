bool hid_has_events(HIDState *hs)
{
    return hs->n > 0 || hs->idle_pending;
}