static ssize_t yurex_read(struct file *file, char __user *buffer, size_t count,
			  loff_t *ppos)
{
	struct usb_yurex *dev;
	int len = 0;
	char in_buffer[20];
	unsigned long flags;

	dev = file->private_data;

	mutex_lock(&dev->io_mutex);
	if (!dev->interface) {		/* already disconnected */
		mutex_unlock(&dev->io_mutex);
		return -ENODEV;
	}

	spin_lock_irqsave(&dev->lock, flags);
	len = snprintf(in_buffer, 20, "%lld\n", dev->bbu);
	spin_unlock_irqrestore(&dev->lock, flags);
	mutex_unlock(&dev->io_mutex);

	if (WARN_ON_ONCE(len >= sizeof(in_buffer)))
		return -EIO;

	return simple_read_from_buffer(buffer, count, ppos, in_buffer, len);
}