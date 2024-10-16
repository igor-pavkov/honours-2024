static void hid_keyboard_event(DeviceState *dev, QemuConsole *src,
                               InputEvent *evt)
{
    HIDState *hs = (HIDState *)dev;
    int scancodes[3], i, count;
    int slot;
    InputKeyEvent *key = evt->u.key.data;

    count = qemu_input_key_value_to_scancode(key->key,
                                             key->down,
                                             scancodes);
    if (hs->n + count > QUEUE_LENGTH) {
        trace_hid_kbd_queue_full();
        return;
    }
    for (i = 0; i < count; i++) {
        slot = (hs->head + hs->n) & QUEUE_MASK; hs->n++;
        hs->kbd.keycodes[slot] = scancodes[i];
    }
    hs->event(hs);
}