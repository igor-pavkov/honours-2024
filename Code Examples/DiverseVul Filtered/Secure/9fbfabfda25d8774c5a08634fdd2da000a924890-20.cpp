static ssize_t part_partition_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", dev_to_bdev(dev)->bd_partno);
}