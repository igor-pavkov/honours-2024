static void adu_release_internal(struct adu_device *dev)
{
	/* decrement our usage count for the device */
	--dev->open_count;
	dev_dbg(&dev->udev->dev, "%s : open count %d\n", __func__,
		dev->open_count);
	if (dev->open_count <= 0) {
		adu_abort_transfers(dev);
		dev->open_count = 0;
	}
}