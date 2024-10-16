static void usb_mask_to_str(char *dest, size_t size,
                            unsigned int speedmask)
{
    static const struct {
        unsigned int mask;
        const char *name;
    } speeds[] = {
        { .mask = USB_SPEED_MASK_FULL,  .name = "full"  },
        { .mask = USB_SPEED_MASK_HIGH,  .name = "high"  },
        { .mask = USB_SPEED_MASK_SUPER, .name = "super" },
    };
    int i, pos = 0;

    for (i = 0; i < ARRAY_SIZE(speeds); i++) {
        if (speeds[i].mask & speedmask) {
            pos += snprintf(dest + pos, size - pos, "%s%s",
                            pos ? "+" : "",
                            speeds[i].name);
        }
    }
}