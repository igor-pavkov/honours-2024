static ssize_t part_alignment_offset_show(struct device *dev,
					  struct device_attribute *attr, char *buf)
{
	struct block_device *bdev = dev_to_bdev(dev);

	return sprintf(buf, "%u\n",
		queue_limit_alignment_offset(&bdev->bd_disk->queue->limits,
				bdev->bd_start_sect));
}