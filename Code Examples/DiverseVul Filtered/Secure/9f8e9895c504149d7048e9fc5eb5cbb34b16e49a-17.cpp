void usb_legacy_register(const char *typename, const char *usbdevice_name,
                         USBDevice *(*usbdevice_init)(USBBus *bus,
                                                      const char *params))
{
    if (usbdevice_name) {
        LegacyUSBFactory *f = g_malloc0(sizeof(*f));
        f->name = typename;
        f->usbdevice_name = usbdevice_name;
        f->usbdevice_init = usbdevice_init;
        legacy_usb_factory = g_slist_append(legacy_usb_factory, f);
    }
}