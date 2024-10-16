static ssize_t ati_remote2_store_channel_mask(struct device *dev,
					      struct device_attribute *attr,
					      const char *buf, size_t count)
{
	struct usb_device *udev = to_usb_device(dev);
	struct usb_interface *intf = usb_ifnum_to_if(udev, 0);
	struct ati_remote2 *ar2 = usb_get_intfdata(intf);
	unsigned int mask;
	int r;

	r = kstrtouint(buf, 0, &mask);
	if (r)
		return r;

	if (mask & ~ATI_REMOTE2_MAX_CHANNEL_MASK)
		return -EINVAL;

	r = usb_autopm_get_interface(ar2->intf[0]);
	if (r) {
		dev_err(&ar2->intf[0]->dev,
			"%s(): usb_autopm_get_interface() = %d\n", __func__, r);
		return r;
	}

	mutex_lock(&ati_remote2_mutex);

	if (mask != ar2->channel_mask) {
		r = ati_remote2_setup(ar2, mask);
		if (!r)
			ar2->channel_mask = mask;
	}

	mutex_unlock(&ati_remote2_mutex);

	usb_autopm_put_interface(ar2->intf[0]);

	return r ? r : count;
}