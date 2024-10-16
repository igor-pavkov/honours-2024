static void b43_chip_reset(struct work_struct *work)
{
	struct b43_wldev *dev =
	    container_of(work, struct b43_wldev, restart_work);
	struct b43_wl *wl = dev->wl;
	int err = 0;
	int prev_status;

	mutex_lock(&wl->mutex);

	prev_status = b43_status(dev);
	/* Bring the device down... */
	if (prev_status >= B43_STAT_STARTED) {
		dev = b43_wireless_core_stop(dev);
		if (!dev) {
			err = -ENODEV;
			goto out;
		}
	}
	if (prev_status >= B43_STAT_INITIALIZED)
		b43_wireless_core_exit(dev);

	/* ...and up again. */
	if (prev_status >= B43_STAT_INITIALIZED) {
		err = b43_wireless_core_init(dev);
		if (err)
			goto out;
	}
	if (prev_status >= B43_STAT_STARTED) {
		err = b43_wireless_core_start(dev);
		if (err) {
			b43_wireless_core_exit(dev);
			goto out;
		}
	}
out:
	if (err)
		wl->current_dev = NULL; /* Failed to init the dev. */
	mutex_unlock(&wl->mutex);

	if (err) {
		b43err(wl, "Controller restart FAILED\n");
		return;
	}

	/* reload configuration */
	b43_op_config(wl->hw, ~0);
	if (wl->vif)
		b43_op_bss_info_changed(wl->hw, wl->vif, &wl->vif->bss_conf, ~0);

	b43info(wl, "Controller restarted\n");
}