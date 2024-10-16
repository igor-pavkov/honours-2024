static int adu_release(struct inode *inode, struct file *file)
{
	struct adu_device *dev;
	int retval = 0;

	if (file == NULL) {
		retval = -ENODEV;
		goto exit;
	}

	dev = file->private_data;
	if (dev == NULL) {
		retval = -ENODEV;
		goto exit;
	}

	mutex_lock(&adutux_mutex); /* not interruptible */

	if (dev->open_count <= 0) {
		dev_dbg(&dev->udev->dev, "%s : device not opened\n", __func__);
		retval = -ENODEV;
		goto unlock;
	}

	adu_release_internal(dev);
	if (dev->udev == NULL) {
		/* the device was unplugged before the file was released */
		if (!dev->open_count)	/* ... and we're the last user */
			adu_delete(dev);
	}
unlock:
	mutex_unlock(&adutux_mutex);
exit:
	return retval;
}