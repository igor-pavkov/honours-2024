static ssize_t part_ro_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", bdev_read_only(dev_to_bdev(dev)));
}