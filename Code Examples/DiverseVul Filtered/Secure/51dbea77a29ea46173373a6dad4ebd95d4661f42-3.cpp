void hid_pointer_activate(HIDState *hs)
{
    if (!hs->ptr.mouse_grabbed) {
        qemu_input_handler_activate(hs->s);
        hs->ptr.mouse_grabbed = 1;
    }
}