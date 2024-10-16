static void disk_del_events(struct gendisk *disk)
{
	if (!disk->ev)
		return;

	disk_block_events(disk);

	mutex_lock(&disk_events_mutex);
	list_del_init(&disk->ev->node);
	mutex_unlock(&disk_events_mutex);

	sysfs_remove_files(&disk_to_dev(disk)->kobj, disk_events_attrs);
}