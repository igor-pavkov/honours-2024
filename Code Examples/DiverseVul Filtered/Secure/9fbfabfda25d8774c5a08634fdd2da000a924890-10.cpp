static void part_release(struct device *dev)
{
	put_disk(dev_to_bdev(dev)->bd_disk);
	iput(dev_to_bdev(dev)->bd_inode);
}