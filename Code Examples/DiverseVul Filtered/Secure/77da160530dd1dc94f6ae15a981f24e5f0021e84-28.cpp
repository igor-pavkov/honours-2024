static void __disk_unblock_events(struct gendisk *disk, bool check_now)
{
	struct disk_events *ev = disk->ev;
	unsigned long intv;
	unsigned long flags;

	spin_lock_irqsave(&ev->lock, flags);

	if (WARN_ON_ONCE(ev->block <= 0))
		goto out_unlock;

	if (--ev->block)
		goto out_unlock;

	intv = disk_events_poll_jiffies(disk);
	if (check_now)
		queue_delayed_work(system_freezable_power_efficient_wq,
				&ev->dwork, 0);
	else if (intv)
		queue_delayed_work(system_freezable_power_efficient_wq,
				&ev->dwork, intv);
out_unlock:
	spin_unlock_irqrestore(&ev->lock, flags);
}