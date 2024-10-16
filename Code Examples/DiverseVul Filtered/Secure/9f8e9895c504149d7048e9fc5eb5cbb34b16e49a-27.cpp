USBDevice *usbdevice_create(const char *cmdline)
{
    USBBus *bus = usb_bus_find(-1 /* any */);
    LegacyUSBFactory *f = NULL;
    GSList *i;
    char driver[32];
    const char *params;
    int len;

    params = strchr(cmdline,':');
    if (params) {
        params++;
        len = params - cmdline;
        if (len > sizeof(driver))
            len = sizeof(driver);
        pstrcpy(driver, len, cmdline);
    } else {
        params = "";
        pstrcpy(driver, sizeof(driver), cmdline);
    }

    for (i = legacy_usb_factory; i; i = i->next) {
        f = i->data;
        if (strcmp(f->usbdevice_name, driver) == 0) {
            break;
        }
    }
    if (i == NULL) {
#if 0
        /* no error because some drivers are not converted (yet) */
        error_report("usbdevice %s not found", driver);
#endif
        return NULL;
    }

    if (!bus) {
        error_report("Error: no usb bus to attach usbdevice %s, "
                     "please try -machine usb=on and check that "
                     "the machine model supports USB", driver);
        return NULL;
    }

    if (!f->usbdevice_init) {
        if (*params) {
            error_report("usbdevice %s accepts no params", driver);
            return NULL;
        }
        return usb_create_simple(bus, f->name);
    }
    return f->usbdevice_init(bus, params);
}