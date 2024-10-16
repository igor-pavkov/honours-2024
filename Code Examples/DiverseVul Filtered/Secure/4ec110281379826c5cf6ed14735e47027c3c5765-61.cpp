static struct block_device *ext4_blkdev_get(dev_t dev)
{
	struct block_device *bdev;
	char b[BDEVNAME_SIZE];

	bdev = open_by_devnum(dev, FMODE_READ|FMODE_WRITE);
	if (IS_ERR(bdev))
		goto fail;
	return bdev;

fail:
	printk(KERN_ERR "EXT4: failed to open journal device %s: %ld\n",
			__bdevname(dev, b), PTR_ERR(bdev));
	return NULL;
}