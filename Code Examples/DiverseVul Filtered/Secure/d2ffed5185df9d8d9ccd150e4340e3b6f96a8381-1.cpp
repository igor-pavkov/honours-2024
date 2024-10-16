static ssize_t driver_override_show(struct device *_dev,
				    struct device_attribute *attr, char *buf)
{
	struct amba_device *dev = to_amba_device(_dev);
	ssize_t len;

	if (!dev->driver_override)
		return 0;

	device_lock(_dev);
	len = sprintf(buf, "%s\n", dev->driver_override);
	device_unlock(_dev);
	return len;
}