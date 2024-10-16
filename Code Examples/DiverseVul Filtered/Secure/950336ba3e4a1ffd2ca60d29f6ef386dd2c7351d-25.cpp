static ssize_t ati_remote2_store_mode_mask(struct device *dev,
					   struct device_attribute *attr,
					   const char *buf, size_t count)
{
	struct usb_device *udev = to_usb_device(dev);
	struct usb_interface *intf = usb_ifnum_to_if(udev, 0);
	struct ati_remote2 *ar2 = usb_get_intfdata(intf);
	unsigned int mask;
	int err;

	err = kstrtouint(buf, 0, &mask);
	if (err)
		return err;

	if (mask & ~ATI_REMOTE2_MAX_MODE_MASK)
		return -EINVAL;

	ar2->mode_mask = mask;

	return count;
}