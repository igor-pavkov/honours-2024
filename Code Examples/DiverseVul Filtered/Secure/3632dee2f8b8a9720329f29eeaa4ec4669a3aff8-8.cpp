static long inotify_ioctl(struct file *file, unsigned int cmd,
			  unsigned long arg)
{
	struct inotify_device *dev;
	void __user *p;
	int ret = -ENOTTY;

	dev = file->private_data;
	p = (void __user *) arg;

	switch (cmd) {
	case FIONREAD:
		ret = put_user(dev->queue_size, (int __user *) p);
		break;
	}

	return ret;
}