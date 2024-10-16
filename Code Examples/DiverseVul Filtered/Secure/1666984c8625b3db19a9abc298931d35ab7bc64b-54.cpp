void usbnet_status_stop(struct usbnet *dev)
{
	if (dev->interrupt) {
		mutex_lock(&dev->interrupt_mutex);
		WARN_ON(dev->interrupt_count == 0);

		if (dev->interrupt_count && --dev->interrupt_count == 0)
			usb_kill_urb(dev->interrupt);

		dev_dbg(&dev->udev->dev,
			"decremented interrupt URB count to %d\n",
			dev->interrupt_count);
		mutex_unlock(&dev->interrupt_mutex);
	}
}