int hid_keyboard_write(HIDState *hs, uint8_t *buf, int len)
{
    if (len > 0) {
        int ledstate = 0;
        /* 0x01: Num Lock LED
         * 0x02: Caps Lock LED
         * 0x04: Scroll Lock LED
         * 0x08: Compose LED
         * 0x10: Kana LED */
        hs->kbd.leds = buf[0];
        if (hs->kbd.leds & 0x04) {
            ledstate |= QEMU_SCROLL_LOCK_LED;
        }
        if (hs->kbd.leds & 0x01) {
            ledstate |= QEMU_NUM_LOCK_LED;
        }
        if (hs->kbd.leds & 0x02) {
            ledstate |= QEMU_CAPS_LOCK_LED;
        }
        kbd_put_ledstate(ledstate);
    }
    return 0;
}