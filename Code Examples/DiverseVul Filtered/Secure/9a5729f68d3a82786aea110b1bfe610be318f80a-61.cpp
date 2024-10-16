static int sisusb_open(struct inode *inode, struct file *file)
{
	struct sisusb_usb_data *sisusb;
	struct usb_interface *interface;
	int subminor = iminor(inode);

	interface = usb_find_interface(&sisusb_driver, subminor);
	if (!interface)
		return -ENODEV;

	sisusb = usb_get_intfdata(interface);
	if (!sisusb)
		return -ENODEV;

	mutex_lock(&sisusb->lock);

	if (!sisusb->present || !sisusb->ready) {
		mutex_unlock(&sisusb->lock);
		return -ENODEV;
	}

	if (sisusb->isopen) {
		mutex_unlock(&sisusb->lock);
		return -EBUSY;
	}

	if (!sisusb->devinit) {
		if (sisusb->sisusb_dev->speed == USB_SPEED_HIGH ||
				sisusb->sisusb_dev->speed >= USB_SPEED_SUPER) {
			if (sisusb_init_gfxdevice(sisusb, 0)) {
				mutex_unlock(&sisusb->lock);
				dev_err(&sisusb->sisusb_dev->dev,
						"Failed to initialize device\n");
				return -EIO;
			}
		} else {
			mutex_unlock(&sisusb->lock);
			dev_err(&sisusb->sisusb_dev->dev,
					"Device not attached to USB 2.0 hub\n");
			return -EIO;
		}
	}

	/* Increment usage count for our sisusb */
	kref_get(&sisusb->kref);

	sisusb->isopen = 1;

	file->private_data = sisusb;

	mutex_unlock(&sisusb->lock);

	return 0;
}