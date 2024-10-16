static void disk_release(struct device *dev)
{
	struct gendisk *disk = dev_to_disk(dev);

	blk_free_devt(dev->devt);
	disk_release_events(disk);
	kfree(disk->random);
	disk_replace_part_tbl(disk, NULL);
	hd_free_part(&disk->part0);
	if (disk->queue)
		blk_put_queue(disk->queue);
	kfree(disk);
}