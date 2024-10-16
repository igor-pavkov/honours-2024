static ssize_t iowarrior_write(struct file *file,
			       const char __user *user_buffer,
			       size_t count, loff_t *ppos)
{
	struct iowarrior *dev;
	int retval = 0;
	char *buf = NULL;	/* for IOW24 and IOW56 we need a buffer */
	struct urb *int_out_urb = NULL;

	dev = file->private_data;

	mutex_lock(&dev->mutex);
	/* verify that the device wasn't unplugged */
	if (!dev->present) {
		retval = -ENODEV;
		goto exit;
	}
	dev_dbg(&dev->interface->dev, "minor %d, count = %zd\n",
		dev->minor, count);
	/* if count is 0 we're already done */
	if (count == 0) {
		retval = 0;
		goto exit;
	}
	/* We only accept full reports */
	if (count != dev->report_size) {
		retval = -EINVAL;
		goto exit;
	}
	switch (dev->product_id) {
	case USB_DEVICE_ID_CODEMERCS_IOW24:
	case USB_DEVICE_ID_CODEMERCS_IOWPV1:
	case USB_DEVICE_ID_CODEMERCS_IOWPV2:
	case USB_DEVICE_ID_CODEMERCS_IOW40:
		/* IOW24 and IOW40 use a synchronous call */
		buf = memdup_user(user_buffer, count);
		if (IS_ERR(buf)) {
			retval = PTR_ERR(buf);
			goto exit;
		}
		retval = usb_set_report(dev->interface, 2, 0, buf, count);
		kfree(buf);
		goto exit;
		break;
	case USB_DEVICE_ID_CODEMERCS_IOW56:
		/* The IOW56 uses asynchronous IO and more urbs */
		if (atomic_read(&dev->write_busy) == MAX_WRITES_IN_FLIGHT) {
			/* Wait until we are below the limit for submitted urbs */
			if (file->f_flags & O_NONBLOCK) {
				retval = -EAGAIN;
				goto exit;
			} else {
				retval = wait_event_interruptible(dev->write_wait,
								  (!dev->present || (atomic_read (&dev-> write_busy) < MAX_WRITES_IN_FLIGHT)));
				if (retval) {
					/* we were interrupted by a signal */
					retval = -ERESTART;
					goto exit;
				}
				if (!dev->present) {
					/* The device was unplugged */
					retval = -ENODEV;
					goto exit;
				}
				if (!dev->opened) {
					/* We were closed while waiting for an URB */
					retval = -ENODEV;
					goto exit;
				}
			}
		}
		atomic_inc(&dev->write_busy);
		int_out_urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!int_out_urb) {
			retval = -ENOMEM;
			goto error_no_urb;
		}
		buf = usb_alloc_coherent(dev->udev, dev->report_size,
					 GFP_KERNEL, &int_out_urb->transfer_dma);
		if (!buf) {
			retval = -ENOMEM;
			dev_dbg(&dev->interface->dev,
				"Unable to allocate buffer\n");
			goto error_no_buffer;
		}
		usb_fill_int_urb(int_out_urb, dev->udev,
				 usb_sndintpipe(dev->udev,
						dev->int_out_endpoint->bEndpointAddress),
				 buf, dev->report_size,
				 iowarrior_write_callback, dev,
				 dev->int_out_endpoint->bInterval);
		int_out_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
		if (copy_from_user(buf, user_buffer, count)) {
			retval = -EFAULT;
			goto error;
		}
		retval = usb_submit_urb(int_out_urb, GFP_KERNEL);
		if (retval) {
			dev_dbg(&dev->interface->dev,
				"submit error %d for urb nr.%d\n",
				retval, atomic_read(&dev->write_busy));
			goto error;
		}
		/* submit was ok */
		retval = count;
		usb_free_urb(int_out_urb);
		goto exit;
		break;
	default:
		/* what do we have here ? An unsupported Product-ID ? */
		dev_err(&dev->interface->dev, "%s - not supported for product=0x%x\n",
			__func__, dev->product_id);
		retval = -EFAULT;
		goto exit;
		break;
	}
error:
	usb_free_coherent(dev->udev, dev->report_size, buf,
			  int_out_urb->transfer_dma);
error_no_buffer:
	usb_free_urb(int_out_urb);
error_no_urb:
	atomic_dec(&dev->write_busy);
	wake_up_interruptible(&dev->write_wait);
exit:
	mutex_unlock(&dev->mutex);
	return retval;
}