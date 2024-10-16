static void __usbnet_status_stop_force(struct usbnet *dev)
{
	if (dev->interrupt) {
		mutex_lock(&dev->interrupt_mutex);
		usb_kill_urb(dev->interrupt);
		dev_dbg(&dev->udev->dev, "killed interrupt URB for suspend\n");
		mutex_unlock(&dev->interrupt_mutex);
	}
}