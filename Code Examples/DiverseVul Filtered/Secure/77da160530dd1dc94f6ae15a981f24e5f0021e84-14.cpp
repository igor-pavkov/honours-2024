static void disk_check_events(struct disk_events *ev,
			      unsigned int *clearing_ptr)
{
	struct gendisk *disk = ev->disk;
	char *envp[ARRAY_SIZE(disk_uevents) + 1] = { };
	unsigned int clearing = *clearing_ptr;
	unsigned int events;
	unsigned long intv;
	int nr_events = 0, i;

	/* check events */
	events = disk->fops->check_events(disk, clearing);

	/* accumulate pending events and schedule next poll if necessary */
	spin_lock_irq(&ev->lock);

	events &= ~ev->pending;
	ev->pending |= events;
	*clearing_ptr &= ~clearing;

	intv = disk_events_poll_jiffies(disk);
	if (!ev->block && intv)
		queue_delayed_work(system_freezable_power_efficient_wq,
				&ev->dwork, intv);

	spin_unlock_irq(&ev->lock);

	/*
	 * Tell userland about new events.  Only the events listed in
	 * @disk->events are reported.  Unlisted events are processed the
	 * same internally but never get reported to userland.
	 */
	for (i = 0; i < ARRAY_SIZE(disk_uevents); i++)
		if (events & disk->events & (1 << i))
			envp[nr_events++] = disk_uevents[i];

	if (nr_events)
		kobject_uevent_env(&disk_to_dev(disk)->kobj, KOBJ_CHANGE, envp);
}