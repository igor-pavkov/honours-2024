static const char *usb_speed(unsigned int speed)
{
    static const char *txt[] = {
        [ USB_SPEED_LOW  ] = "1.5",
        [ USB_SPEED_FULL ] = "12",
        [ USB_SPEED_HIGH ] = "480",
        [ USB_SPEED_SUPER ] = "5000",
    };
    if (speed >= ARRAY_SIZE(txt))
        return "?";
    return txt[speed];
}