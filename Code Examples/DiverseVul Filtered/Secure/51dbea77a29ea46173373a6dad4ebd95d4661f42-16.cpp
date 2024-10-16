int hid_keyboard_poll(HIDState *hs, uint8_t *buf, int len)
{
    hs->idle_pending = false;

    if (len < 2) {
        return 0;
    }

    hid_keyboard_process_keycode(hs);

    buf[0] = hs->kbd.modifiers & 0xff;
    buf[1] = 0;
    if (hs->kbd.keys > 6) {
        memset(buf + 2, HID_USAGE_ERROR_ROLLOVER, MIN(8, len) - 2);
    } else {
        memcpy(buf + 2, hs->kbd.key, MIN(8, len) - 2);
    }

    return MIN(8, len);
}