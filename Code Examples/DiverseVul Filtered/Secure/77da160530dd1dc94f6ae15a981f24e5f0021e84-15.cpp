static void disk_alloc_events(struct gendisk *disk)
{
	struct disk_events *ev;

	if (!disk->fops->check_events)
		return;

	ev = kzalloc(sizeof(*ev), GFP_KERNEL);
	if (!ev) {
		pr_warn("%s: failed to initialize events\n", disk->disk_name);
		return;
	}

	INIT_LIST_HEAD(&ev->node);
	ev->disk = disk;
	spin_lock_init(&ev->lock);
	mutex_init(&ev->block_mutex);
	ev->block = 1;
	ev->poll_msecs = -1;
	INIT_DELAYED_WORK(&ev->dwork, disk_events_workfn);

	disk->ev = ev;
}