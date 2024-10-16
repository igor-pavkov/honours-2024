static ssize_t tower_read (struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
	struct lego_usb_tower *dev;
	size_t bytes_to_read;
	int i;
	int retval = 0;
	unsigned long timeout = 0;

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

	/* verify that we actually have some data to read */
	if (count == 0) {
		dev_dbg(&dev->udev->dev, "read request of 0 bytes\n");
		goto unlock_exit;
	}

	if (read_timeout) {
		timeout = jiffies + msecs_to_jiffies(read_timeout);
	}

	/* wait for data */
	tower_check_for_read_packet (dev);
	while (dev->read_packet_length == 0) {
		if (file->f_flags & O_NONBLOCK) {
			retval = -EAGAIN;
			goto unlock_exit;
		}
		retval = wait_event_interruptible_timeout(dev->read_wait, dev->interrupt_in_done, dev->packet_timeout_jiffies);
		if (retval < 0) {
			goto unlock_exit;
		}

		/* reset read timeout during read or write activity */
		if (read_timeout
		    && (dev->read_buffer_length || dev->interrupt_out_busy)) {
			timeout = jiffies + msecs_to_jiffies(read_timeout);
		}
		/* check for read timeout */
		if (read_timeout && time_after (jiffies, timeout)) {
			retval = -ETIMEDOUT;
			goto unlock_exit;
		}
		tower_check_for_read_packet (dev);
	}

	/* copy the data from read_buffer into userspace */
	bytes_to_read = min(count, dev->read_packet_length);

	if (copy_to_user (buffer, dev->read_buffer, bytes_to_read)) {
		retval = -EFAULT;
		goto unlock_exit;
	}

	spin_lock_irq (&dev->read_buffer_lock);
	dev->read_buffer_length -= bytes_to_read;
	dev->read_packet_length -= bytes_to_read;
	for (i=0; i<dev->read_buffer_length; i++) {
		dev->read_buffer[i] = dev->read_buffer[i+bytes_to_read];
	}
	spin_unlock_irq (&dev->read_buffer_lock);

	retval = bytes_to_read;

unlock_exit:
	/* unlock the device */
	mutex_unlock(&dev->lock);

exit:
	return retval;
}