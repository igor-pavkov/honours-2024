static ssize_t tower_write (struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	struct lego_usb_tower *dev;
	size_t bytes_to_write;
	int retval = 0;

	dev = file->private_data;

	/* lock this object */
	if (mutex_lock_interruptible(&dev->lock)) {
		retval = -ERESTARTSYS;
		goto exit;
	}

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		retval = -ENODEV;
		pr_err("No device or device unplugged %d\n", retval);
		goto unlock_exit;
	}

	/* verify that we actually have some data to write */
	if (count == 0) {
		dev_dbg(&dev->udev->dev, "write request of 0 bytes\n");
		goto unlock_exit;
	}

	/* wait until previous transfer is finished */
	while (dev->interrupt_out_busy) {
		if (file->f_flags & O_NONBLOCK) {
			retval = -EAGAIN;
			goto unlock_exit;
		}
		retval = wait_event_interruptible (dev->write_wait, !dev->interrupt_out_busy);
		if (retval) {
			goto unlock_exit;
		}
	}

	/* write the data into interrupt_out_buffer from userspace */
	bytes_to_write = min_t(int, count, write_buffer_size);
	dev_dbg(&dev->udev->dev, "%s: count = %Zd, bytes_to_write = %Zd\n",
		__func__, count, bytes_to_write);

	if (copy_from_user (dev->interrupt_out_buffer, buffer, bytes_to_write)) {
		retval = -EFAULT;
		goto unlock_exit;
	}

	/* send off the urb */
	usb_fill_int_urb(dev->interrupt_out_urb,
			 dev->udev,
			 usb_sndintpipe(dev->udev, dev->interrupt_out_endpoint->bEndpointAddress),
			 dev->interrupt_out_buffer,
			 bytes_to_write,
			 tower_interrupt_out_callback,
			 dev,
			 dev->interrupt_out_interval);

	dev->interrupt_out_busy = 1;
	wmb();

	retval = usb_submit_urb (dev->interrupt_out_urb, GFP_KERNEL);
	if (retval) {
		dev->interrupt_out_busy = 0;
		dev_err(&dev->udev->dev,
			"Couldn't submit interrupt_out_urb %d\n", retval);
		goto unlock_exit;
	}
	retval = bytes_to_write;

unlock_exit:
	/* unlock the device */
	mutex_unlock(&dev->lock);

exit:
	return retval;
}