static long iowarrior_ioctl(struct file *file, unsigned int cmd,
							unsigned long arg)
{
	struct iowarrior *dev = NULL;
	__u8 *buffer;
	__u8 __user *user_buffer;
	int retval;
	int io_res;		/* checks for bytes read/written and copy_to/from_user results */

	dev = file->private_data;
	if (!dev)
		return -ENODEV;

	buffer = kzalloc(dev->report_size, GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	/* lock this object */
	mutex_lock(&iowarrior_mutex);
	mutex_lock(&dev->mutex);

	/* verify that the device wasn't unplugged */
	if (!dev->present) {
		retval = -ENODEV;
		goto error_out;
	}

	dev_dbg(&dev->interface->dev, "minor %d, cmd 0x%.4x, arg %ld\n",
		dev->minor, cmd, arg);

	retval = 0;
	io_res = 0;
	switch (cmd) {
	case IOW_WRITE:
		if (dev->product_id == USB_DEVICE_ID_CODEMERCS_IOW24 ||
		    dev->product_id == USB_DEVICE_ID_CODEMERCS_IOWPV1 ||
		    dev->product_id == USB_DEVICE_ID_CODEMERCS_IOWPV2 ||
		    dev->product_id == USB_DEVICE_ID_CODEMERCS_IOW40) {
			user_buffer = (__u8 __user *)arg;
			io_res = copy_from_user(buffer, user_buffer,
						dev->report_size);
			if (io_res) {
				retval = -EFAULT;
			} else {
				io_res = usb_set_report(dev->interface, 2, 0,
							buffer,
							dev->report_size);
				if (io_res < 0)
					retval = io_res;
			}
		} else {
			retval = -EINVAL;
			dev_err(&dev->interface->dev,
				"ioctl 'IOW_WRITE' is not supported for product=0x%x.\n",
				dev->product_id);
		}
		break;
	case IOW_READ:
		user_buffer = (__u8 __user *)arg;
		io_res = usb_get_report(dev->udev,
					dev->interface->cur_altsetting, 1, 0,
					buffer, dev->report_size);
		if (io_res < 0)
			retval = io_res;
		else {
			io_res = copy_to_user(user_buffer, buffer, dev->report_size);
			if (io_res)
				retval = -EFAULT;
		}
		break;
	case IOW_GETINFO:
		{
			/* Report available information for the device */
			struct iowarrior_info info;
			/* needed for power consumption */
			struct usb_config_descriptor *cfg_descriptor = &dev->udev->actconfig->desc;

			memset(&info, 0, sizeof(info));
			/* directly from the descriptor */
			info.vendor = le16_to_cpu(dev->udev->descriptor.idVendor);
			info.product = dev->product_id;
			info.revision = le16_to_cpu(dev->udev->descriptor.bcdDevice);

			/* 0==UNKNOWN, 1==LOW(usb1.1) ,2=FULL(usb1.1), 3=HIGH(usb2.0) */
			info.speed = dev->udev->speed;
			info.if_num = dev->interface->cur_altsetting->desc.bInterfaceNumber;
			info.report_size = dev->report_size;

			/* serial number string has been read earlier 8 chars or empty string */
			memcpy(info.serial, dev->chip_serial,
			       sizeof(dev->chip_serial));
			if (cfg_descriptor == NULL) {
				info.power = -1;	/* no information available */
			} else {
				/* the MaxPower is stored in units of 2mA to make it fit into a byte-value */
				info.power = cfg_descriptor->bMaxPower * 2;
			}
			io_res = copy_to_user((struct iowarrior_info __user *)arg, &info,
					 sizeof(struct iowarrior_info));
			if (io_res)
				retval = -EFAULT;
			break;
		}
	default:
		/* return that we did not understand this ioctl call */
		retval = -ENOTTY;
		break;
	}
error_out:
	/* unlock the device */
	mutex_unlock(&dev->mutex);
	mutex_unlock(&iowarrior_mutex);
	kfree(buffer);
	return retval;
}