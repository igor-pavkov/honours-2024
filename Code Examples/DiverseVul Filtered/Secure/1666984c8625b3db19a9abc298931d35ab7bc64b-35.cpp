int usbnet_status_start(struct usbnet *dev, gfp_t mem_flags)
{
	int ret = 0;

	WARN_ON_ONCE(dev->interrupt == NULL);
	if (dev->interrupt) {
		mutex_lock(&dev->interrupt_mutex);

		if (++dev->interrupt_count == 1)
			ret = usb_submit_urb(dev->interrupt, mem_flags);

		dev_dbg(&dev->udev->dev, "incremented interrupt URB count to %d\n",
			dev->interrupt_count);
		mutex_unlock(&dev->interrupt_mutex);
	}
	return ret;
}