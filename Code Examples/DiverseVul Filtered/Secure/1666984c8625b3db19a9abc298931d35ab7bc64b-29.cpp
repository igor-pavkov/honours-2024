static int __usbnet_status_start_force(struct usbnet *dev, gfp_t mem_flags)
{
	int ret = 0;

	mutex_lock(&dev->interrupt_mutex);
	if (dev->interrupt_count) {
		ret = usb_submit_urb(dev->interrupt, mem_flags);
		dev_dbg(&dev->udev->dev,
			"submitted interrupt URB for resume\n");
	}
	mutex_unlock(&dev->interrupt_mutex);
	return ret;
}