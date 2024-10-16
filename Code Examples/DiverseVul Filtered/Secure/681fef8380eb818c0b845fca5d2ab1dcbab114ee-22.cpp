static long usbdev_compat_ioctl(struct file *file, unsigned int cmd,
			unsigned long arg)
{
	int ret;

	ret = usbdev_do_ioctl(file, cmd, compat_ptr(arg));

	return ret;
}