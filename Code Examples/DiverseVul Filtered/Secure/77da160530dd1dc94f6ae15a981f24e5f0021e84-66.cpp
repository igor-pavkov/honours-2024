void disk_unblock_events(struct gendisk *disk)
{
	if (disk->ev)
		__disk_unblock_events(disk, false);
}