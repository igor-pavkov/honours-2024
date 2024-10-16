static char *usb_get_fw_dev_path(DeviceState *qdev)
{
    USBDevice *dev = USB_DEVICE(qdev);
    char *fw_path, *in;
    ssize_t pos = 0, fw_len;
    long nr;

    fw_len = 32 + strlen(dev->port->path) * 6;
    fw_path = g_malloc(fw_len);
    in = dev->port->path;
    while (fw_len - pos > 0) {
        nr = strtol(in, &in, 10);
        if (in[0] == '.') {
            /* some hub between root port and device */
            pos += snprintf(fw_path + pos, fw_len - pos, "hub@%ld/", nr);
            in++;
        } else {
            /* the device itself */
            pos += snprintf(fw_path + pos, fw_len - pos, "%s@%ld",
                            qdev_fw_name(qdev), nr);
            break;
        }
    }
    return fw_path;
}