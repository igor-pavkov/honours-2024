static int adu_open(struct inode *inode, struct file *file)
{
	struct adu_device *dev = NULL;
	struct usb_interface *interface;
	int subminor;
	int retval;

	subminor = iminor(inode);

	retval = mutex_lock_interruptible(&adutux_mutex);
	if (retval)
		goto exit_no_lock;

	interface = usb_find_interface(&adu_driver, subminor);
	if (!interface) {
		pr_err("%s - error, can't find device for minor %d\n",
		       __func__, subminor);
		retval = -ENODEV;
		goto exit_no_device;
	}

	dev = usb_get_intfdata(interface);
	if (!dev || !dev->udev) {
		retval = -ENODEV;
		goto exit_no_device;
	}

	/* check that nobody else is using the device */
	if (dev->open_count) {
		retval = -EBUSY;
		goto exit_no_device;
	}

	++dev->open_count;
	dev_dbg(&dev->udev->dev, "%s: open count %d\n", __func__,
		dev->open_count);

	/* save device in the file's private structure */
	file->private_data = dev;

	/* initialize in direction */
	dev->read_buffer_length = 0;

	/* fixup first read by having urb waiting for it */
	usb_fill_int_urb(dev->interrupt_in_urb, dev->udev,
			 usb_rcvintpipe(dev->udev,
					dev->interrupt_in_endpoint->bEndpointAddress),
			 dev->interrupt_in_buffer,
			 usb_endpoint_maxp(dev->interrupt_in_endpoint),
			 adu_interrupt_in_callback, dev,
			 dev->interrupt_in_endpoint->bInterval);
	dev->read_urb_finished = 0;
	if (usb_submit_urb(dev->interrupt_in_urb, GFP_KERNEL))
		dev->read_urb_finished = 1;
	/* we ignore failure */
	/* end of fixup for first read */

	/* initialize out direction */
	dev->out_urb_finished = 1;

	retval = 0;

exit_no_device:
	mutex_unlock(&adutux_mutex);
exit_no_lock:
	return retval;
}