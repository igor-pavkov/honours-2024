static ssize_t adu_read(struct file *file, __user char *buffer, size_t count,
			loff_t *ppos)
{
	struct adu_device *dev;
	size_t bytes_read = 0;
	size_t bytes_to_read = count;
	int retval = 0;
	int timeout = 0;
	int should_submit = 0;
	unsigned long flags;
	DECLARE_WAITQUEUE(wait, current);

	dev = file->private_data;
	if (mutex_lock_interruptible(&dev->mtx))
		return -ERESTARTSYS;

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		retval = -ENODEV;
		pr_err("No device or device unplugged %d\n", retval);
		goto exit;
	}

	/* verify that some data was requested */
	if (count == 0) {
		dev_dbg(&dev->udev->dev, "%s : read request of 0 bytes\n",
			__func__);
		goto exit;
	}

	timeout = COMMAND_TIMEOUT;
	dev_dbg(&dev->udev->dev, "%s : about to start looping\n", __func__);
	while (bytes_to_read) {
		size_t data_in_secondary = dev->secondary_tail - dev->secondary_head;
		dev_dbg(&dev->udev->dev,
			"%s : while, data_in_secondary=%zu, status=%d\n",
			__func__, data_in_secondary,
			dev->interrupt_in_urb->status);

		if (data_in_secondary) {
			/* drain secondary buffer */
			size_t amount = min(bytes_to_read, data_in_secondary);
			if (copy_to_user(buffer, dev->read_buffer_secondary+dev->secondary_head, amount)) {
				retval = -EFAULT;
				goto exit;
			}
			dev->secondary_head += amount;
			bytes_read += amount;
			bytes_to_read -= amount;
		} else {
			/* we check the primary buffer */
			spin_lock_irqsave (&dev->buflock, flags);
			if (dev->read_buffer_length) {
				/* we secure access to the primary */
				char *tmp;
				dev_dbg(&dev->udev->dev,
					"%s : swap, read_buffer_length = %d\n",
					__func__, dev->read_buffer_length);
				tmp = dev->read_buffer_secondary;
				dev->read_buffer_secondary = dev->read_buffer_primary;
				dev->read_buffer_primary = tmp;
				dev->secondary_head = 0;
				dev->secondary_tail = dev->read_buffer_length;
				dev->read_buffer_length = 0;
				spin_unlock_irqrestore(&dev->buflock, flags);
				/* we have a free buffer so use it */
				should_submit = 1;
			} else {
				/* even the primary was empty - we may need to do IO */
				if (!dev->read_urb_finished) {
					/* somebody is doing IO */
					spin_unlock_irqrestore(&dev->buflock, flags);
					dev_dbg(&dev->udev->dev,
						"%s : submitted already\n",
						__func__);
				} else {
					/* we must initiate input */
					dev_dbg(&dev->udev->dev,
						"%s : initiate input\n",
						__func__);
					dev->read_urb_finished = 0;
					spin_unlock_irqrestore(&dev->buflock, flags);

					usb_fill_int_urb(dev->interrupt_in_urb, dev->udev,
							usb_rcvintpipe(dev->udev,
								dev->interrupt_in_endpoint->bEndpointAddress),
							 dev->interrupt_in_buffer,
							 usb_endpoint_maxp(dev->interrupt_in_endpoint),
							 adu_interrupt_in_callback,
							 dev,
							 dev->interrupt_in_endpoint->bInterval);
					retval = usb_submit_urb(dev->interrupt_in_urb, GFP_KERNEL);
					if (retval) {
						dev->read_urb_finished = 1;
						if (retval == -ENOMEM) {
							retval = bytes_read ? bytes_read : -ENOMEM;
						}
						dev_dbg(&dev->udev->dev,
							"%s : submit failed\n",
							__func__);
						goto exit;
					}
				}

				/* we wait for I/O to complete */
				set_current_state(TASK_INTERRUPTIBLE);
				add_wait_queue(&dev->read_wait, &wait);
				spin_lock_irqsave(&dev->buflock, flags);
				if (!dev->read_urb_finished) {
					spin_unlock_irqrestore(&dev->buflock, flags);
					timeout = schedule_timeout(COMMAND_TIMEOUT);
				} else {
					spin_unlock_irqrestore(&dev->buflock, flags);
					set_current_state(TASK_RUNNING);
				}
				remove_wait_queue(&dev->read_wait, &wait);

				if (timeout <= 0) {
					dev_dbg(&dev->udev->dev,
						"%s : timeout\n", __func__);
					retval = bytes_read ? bytes_read : -ETIMEDOUT;
					goto exit;
				}

				if (signal_pending(current)) {
					dev_dbg(&dev->udev->dev,
						"%s : signal pending\n",
						__func__);
					retval = bytes_read ? bytes_read : -EINTR;
					goto exit;
				}
			}
		}
	}

	retval = bytes_read;
	/* if the primary buffer is empty then use it */
	spin_lock_irqsave(&dev->buflock, flags);
	if (should_submit && dev->read_urb_finished) {
		dev->read_urb_finished = 0;
		spin_unlock_irqrestore(&dev->buflock, flags);
		usb_fill_int_urb(dev->interrupt_in_urb, dev->udev,
				 usb_rcvintpipe(dev->udev,
					dev->interrupt_in_endpoint->bEndpointAddress),
				dev->interrupt_in_buffer,
				usb_endpoint_maxp(dev->interrupt_in_endpoint),
				adu_interrupt_in_callback,
				dev,
				dev->interrupt_in_endpoint->bInterval);
		if (usb_submit_urb(dev->interrupt_in_urb, GFP_KERNEL) != 0)
			dev->read_urb_finished = 1;
		/* we ignore failure */
	} else {
		spin_unlock_irqrestore(&dev->buflock, flags);
	}

exit:
	/* unlock the device */
	mutex_unlock(&dev->mtx);

	return retval;
}