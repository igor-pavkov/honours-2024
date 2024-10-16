static void b43_beacon_update_trigger_work(struct work_struct *work)
{
	struct b43_wl *wl = container_of(work, struct b43_wl,
					 beacon_update_trigger);
	struct b43_wldev *dev;

	mutex_lock(&wl->mutex);
	dev = wl->current_dev;
	if (likely(dev && (b43_status(dev) >= B43_STAT_INITIALIZED))) {
		if (b43_bus_host_is_sdio(dev->dev)) {
			/* wl->mutex is enough. */
			b43_do_beacon_update_trigger_work(dev);
			mmiowb();
		} else {
			spin_lock_irq(&wl->hardirq_lock);
			b43_do_beacon_update_trigger_work(dev);
			mmiowb();
			spin_unlock_irq(&wl->hardirq_lock);
		}
	}
	mutex_unlock(&wl->mutex);
}