static unsigned int v4l2_loopback_poll(struct file *file, struct poll_table_struct *pts)
{
	struct v4l2_loopback_opener *opener;
	struct v4l2_loopback_device *dev;
	int ret_mask = 0;
	MARK();

	opener = file->private_data;
	dev    = v4l2loopback_getdevice(file);

	switch (opener->type) {
	case WRITER:
		ret_mask = POLLOUT | POLLWRNORM;
		break;
	case READER:
		poll_wait(file, &dev->read_event, pts);
		if (can_read(dev, opener))
			ret_mask =  POLLIN | POLLRDNORM;
		break;
	default:
		ret_mask = -POLLERR;
	}
	MARK();

	return ret_mask;
}