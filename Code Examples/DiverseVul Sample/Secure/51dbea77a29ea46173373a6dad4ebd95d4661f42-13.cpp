void hid_init(HIDState *hs, int kind, HIDEventFunc event)
{
    hs->kind = kind;
    hs->event = event;

    if (hs->kind == HID_KEYBOARD) {
        hs->s = qemu_input_handler_register((DeviceState *)hs,
                                            &hid_keyboard_handler);
        qemu_input_handler_activate(hs->s);
    } else if (hs->kind == HID_MOUSE) {
        hs->s = qemu_input_handler_register((DeviceState *)hs,
                                            &hid_mouse_handler);
    } else if (hs->kind == HID_TABLET) {
        hs->s = qemu_input_handler_register((DeviceState *)hs,
                                            &hid_tablet_handler);
    }
}