static int descriptors_changed(struct usb_device *udev,
		struct usb_device_descriptor *old_device_descriptor,
		struct usb_host_bos *old_bos)
{
	int		changed = 0;
	unsigned	index;
	unsigned	serial_len = 0;
	unsigned	len;
	unsigned	old_length;
	int		length;
	char		*buf;

	if (memcmp(&udev->descriptor, old_device_descriptor,
			sizeof(*old_device_descriptor)) != 0)
		return 1;

	if ((old_bos && !udev->bos) || (!old_bos && udev->bos))
		return 1;
	if (udev->bos) {
		len = le16_to_cpu(udev->bos->desc->wTotalLength);
		if (len != le16_to_cpu(old_bos->desc->wTotalLength))
			return 1;
		if (memcmp(udev->bos->desc, old_bos->desc, len))
			return 1;
	}

	/* Since the idVendor, idProduct, and bcdDevice values in the
	 * device descriptor haven't changed, we will assume the
	 * Manufacturer and Product strings haven't changed either.
	 * But the SerialNumber string could be different (e.g., a
	 * different flash card of the same brand).
	 */
	if (udev->serial)
		serial_len = strlen(udev->serial) + 1;

	len = serial_len;
	for (index = 0; index < udev->descriptor.bNumConfigurations; index++) {
		old_length = le16_to_cpu(udev->config[index].desc.wTotalLength);
		len = max(len, old_length);
	}

	buf = kmalloc(len, GFP_NOIO);
	if (buf == NULL) {
		dev_err(&udev->dev, "no mem to re-read configs after reset\n");
		/* assume the worst */
		return 1;
	}
	for (index = 0; index < udev->descriptor.bNumConfigurations; index++) {
		old_length = le16_to_cpu(udev->config[index].desc.wTotalLength);
		length = usb_get_descriptor(udev, USB_DT_CONFIG, index, buf,
				old_length);
		if (length != old_length) {
			dev_dbg(&udev->dev, "config index %d, error %d\n",
					index, length);
			changed = 1;
			break;
		}
		if (memcmp(buf, udev->rawdescriptors[index], old_length)
				!= 0) {
			dev_dbg(&udev->dev, "config index %d changed (#%d)\n",
				index,
				((struct usb_config_descriptor *) buf)->
					bConfigurationValue);
			changed = 1;
			break;
		}
	}

	if (!changed && serial_len) {
		length = usb_string(udev, udev->descriptor.iSerialNumber,
				buf, serial_len);
		if (length + 1 != serial_len) {
			dev_dbg(&udev->dev, "serial string error %d\n",
					length);
			changed = 1;
		} else if (memcmp(buf, udev->serial, length) != 0) {
			dev_dbg(&udev->dev, "serial string changed\n");
			changed = 1;
		}
	}

	kfree(buf);
	return changed;
}