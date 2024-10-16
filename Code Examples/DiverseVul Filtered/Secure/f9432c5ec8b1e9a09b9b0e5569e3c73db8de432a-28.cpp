static ssize_t show_channel(struct device *tty_dev, struct device_attribute *attr, char *buf)
{
	struct rfcomm_dev *dev = dev_get_drvdata(tty_dev);
	return sprintf(buf, "%d\n", dev->channel);
}