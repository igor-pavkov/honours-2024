static ssize_t disk_capability_show(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);

	return sprintf(buf, "%x\n", disk->flags);
}