static void disk_add_events(struct gendisk *disk)
{
	if (!disk->ev)
		return;

	/* FIXME: error handling */
	if (sysfs_create_files(&disk_to_dev(disk)->kobj, disk_events_attrs) < 0)
		pr_warn("%s: failed to create sysfs files for events\n",
			disk->disk_name);

	mutex_lock(&disk_events_mutex);
	list_add_tail(&disk->ev->node, &disk_events);
	mutex_unlock(&disk_events_mutex);

	/*
	 * Block count is initialized to 1 and the following initial
	 * unblock kicks it into action.
	 */
	__disk_unblock_events(disk, true);
}