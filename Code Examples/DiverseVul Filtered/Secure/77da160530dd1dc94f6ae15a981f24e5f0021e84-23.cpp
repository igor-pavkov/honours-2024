static void disk_release_events(struct gendisk *disk)
{
	/* the block count should be 1 from disk_del_events() */
	WARN_ON_ONCE(disk->ev && disk->ev->block != 1);
	kfree(disk->ev);
}