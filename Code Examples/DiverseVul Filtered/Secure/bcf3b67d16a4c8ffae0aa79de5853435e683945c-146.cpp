static ssize_t support_device_change_show(struct device_driver *dd, char *buf)
{
	return sprintf(buf, "%u\n", support_device_change);
}