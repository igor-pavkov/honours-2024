static ssize_t iowarrior_read(struct file *file, char __user *buffer,
			      size_t count, loff_t *ppos)
{
	struct iowarrior *dev;
	int read_idx;
	int offset;

	dev = file->private_data;

	/* verify that the device wasn't unplugged */
	if (!dev || !dev->present)
		return -ENODEV;

	dev_dbg(&dev->interface->dev, "minor %d, count = %zd\n",
		dev->minor, count);

	/* read count must be packet size (+ time stamp) */
	if ((count != dev->report_size)
	    && (count != (dev->report_size + 1)))
		return -EINVAL;

	/* repeat until no buffer overrun in callback handler occur */
	do {
		atomic_set(&dev->overflow_flag, 0);
		if ((read_idx = read_index(dev)) == -1) {
			/* queue empty */
			if (file->f_flags & O_NONBLOCK)
				return -EAGAIN;
			else {
				//next line will return when there is either new data, or the device is unplugged
				int r = wait_event_interruptible(dev->read_wait,
								 (!dev->present
								  || (read_idx =
								      read_index
								      (dev)) !=
								  -1));
				if (r) {
					//we were interrupted by a signal
					return -ERESTART;
				}
				if (!dev->present) {
					//The device was unplugged
					return -ENODEV;
				}
				if (read_idx == -1) {
					// Can this happen ???
					return 0;
				}
			}
		}

		offset = read_idx * (dev->report_size + 1);
		if (copy_to_user(buffer, dev->read_queue + offset, count)) {
			return -EFAULT;
		}
	} while (atomic_read(&dev->overflow_flag));

	read_idx = ++read_idx == MAX_INTERRUPT_BUFFER ? 0 : read_idx;
	atomic_set(&dev->read_idx, read_idx);
	return count;
}