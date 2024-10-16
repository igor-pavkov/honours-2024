static long sisusb_compat_ioctl(struct file *f, unsigned int cmd,
		unsigned long arg)
{
	switch (cmd) {
	case SISUSB_GET_CONFIG_SIZE:
	case SISUSB_GET_CONFIG:
	case SISUSB_COMMAND:
		return sisusb_ioctl(f, cmd, arg);

	default:
		return -ENOIOCTLCMD;
	}
}