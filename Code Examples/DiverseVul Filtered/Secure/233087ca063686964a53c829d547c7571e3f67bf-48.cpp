static int floppy_alloc_disk(unsigned int drive, unsigned int type)
{
	struct gendisk *disk;

	disk = blk_mq_alloc_disk(&tag_sets[drive], NULL);
	if (IS_ERR(disk))
		return PTR_ERR(disk);

	blk_queue_max_hw_sectors(disk->queue, 64);
	disk->major = FLOPPY_MAJOR;
	disk->first_minor = TOMINOR(drive) | (type << 2);
	disk->minors = 1;
	disk->fops = &floppy_fops;
	disk->flags |= GENHD_FL_NO_PART;
	disk->events = DISK_EVENT_MEDIA_CHANGE;
	if (type)
		sprintf(disk->disk_name, "fd%d_type%d", drive, type);
	else
		sprintf(disk->disk_name, "fd%d", drive);
	/* to be cleaned up... */
	disk->private_data = (void *)(long)drive;
	disk->flags |= GENHD_FL_REMOVABLE;

	disks[drive][type] = disk;
	return 0;
}