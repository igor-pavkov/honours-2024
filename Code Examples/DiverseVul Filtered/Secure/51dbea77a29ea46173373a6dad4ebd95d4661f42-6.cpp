void hid_free(HIDState *hs)
{
    qemu_input_handler_unregister(hs->s);
    hid_del_idle_timer(hs);
}