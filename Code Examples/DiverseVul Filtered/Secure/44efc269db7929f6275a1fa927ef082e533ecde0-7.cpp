static ssize_t adu_write(struct file *file, const __user char *buffer,
			 size_t count, loff_t *ppos)
{
	DECLARE_WAITQUEUE(waita, current);
	struct adu_device *dev;
	size_t bytes_written = 0;
	size_t bytes_to_write;
	size_t buffer_size;
	unsigned long flags;
	int retval;

	dev = file->private_data;

	retval = mutex_lock_interruptible(&dev->mtx);
	if (retval)
		goto exit_nolock;

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		retval = -ENODEV;
		pr_err("No device or device unplugged %d\n", retval);
		goto exit;
	}

	/* verify that we actually have some data to write */
	if (count == 0) {
		dev_dbg(&dev->udev->dev, "%s : write request of 0 bytes\n",
			__func__);
		goto exit;
	}

	while (count > 0) {
		add_wait_queue(&dev->write_wait, &waita);
		set_current_state(TASK_INTERRUPTIBLE);
		spin_lock_irqsave(&dev->buflock, flags);
		if (!dev->out_urb_finished) {
			spin_unlock_irqrestore(&dev->buflock, flags);

			mutex_unlock(&dev->mtx);
			if (signal_pending(current)) {
				dev_dbg(&dev->udev->dev, "%s : interrupted\n",
					__func__);
				set_current_state(TASK_RUNNING);
				retval = -EINTR;
				goto exit_onqueue;
			}
			if (schedule_timeout(COMMAND_TIMEOUT) == 0) {
				dev_dbg(&dev->udev->dev,
					"%s - command timed out.\n", __func__);
				retval = -ETIMEDOUT;
				goto exit_onqueue;
			}
			remove_wait_queue(&dev->write_wait, &waita);
			retval = mutex_lock_interruptible(&dev->mtx);
			if (retval) {
				retval = bytes_written ? bytes_written : retval;
				goto exit_nolock;
			}

			dev_dbg(&dev->udev->dev,
				"%s : in progress, count = %zd\n",
				__func__, count);
		} else {
			spin_unlock_irqrestore(&dev->buflock, flags);
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&dev->write_wait, &waita);
			dev_dbg(&dev->udev->dev, "%s : sending, count = %zd\n",
				__func__, count);

			/* write the data into interrupt_out_buffer from userspace */
			buffer_size = usb_endpoint_maxp(dev->interrupt_out_endpoint);
			bytes_to_write = count > buffer_size ? buffer_size : count;
			dev_dbg(&dev->udev->dev,
				"%s : buffer_size = %zd, count = %zd, bytes_to_write = %zd\n",
				__func__, buffer_size, count, bytes_to_write);

			if (copy_from_user(dev->interrupt_out_buffer, buffer, bytes_to_write) != 0) {
				retval = -EFAULT;
				goto exit;
			}

			/* send off the urb */
			usb_fill_int_urb(
				dev->interrupt_out_urb,
				dev->udev,
				usb_sndintpipe(dev->udev, dev->interrupt_out_endpoint->bEndpointAddress),
				dev->interrupt_out_buffer,
				bytes_to_write,
				adu_interrupt_out_callback,
				dev,
				dev->interrupt_out_endpoint->bInterval);
			dev->interrupt_out_urb->actual_length = bytes_to_write;
			dev->out_urb_finished = 0;
			retval = usb_submit_urb(dev->interrupt_out_urb, GFP_KERNEL);
			if (retval < 0) {
				dev->out_urb_finished = 1;
				dev_err(&dev->udev->dev, "Couldn't submit "
					"interrupt_out_urb %d\n", retval);
				goto exit;
			}

			buffer += bytes_to_write;
			count -= bytes_to_write;

			bytes_written += bytes_to_write;
		}
	}
	mutex_unlock(&dev->mtx);
	return bytes_written;

exit:
	mutex_unlock(&dev->mtx);
exit_nolock:
	return retval;

exit_onqueue:
	remove_wait_queue(&dev->write_wait, &waita);
	return retval;
}