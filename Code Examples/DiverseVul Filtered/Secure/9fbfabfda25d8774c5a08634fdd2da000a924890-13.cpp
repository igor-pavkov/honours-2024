static ssize_t part_start_show(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%llu\n", dev_to_bdev(dev)->bd_start_sect);
}