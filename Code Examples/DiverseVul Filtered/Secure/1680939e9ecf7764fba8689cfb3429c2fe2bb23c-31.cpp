static int rpmsg_remove_device(struct device *dev, void *data)
{
	device_unregister(dev);

	return 0;
}