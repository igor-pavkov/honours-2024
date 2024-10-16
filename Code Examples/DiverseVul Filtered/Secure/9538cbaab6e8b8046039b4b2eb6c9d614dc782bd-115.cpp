static void b43_ssb_remove(struct ssb_device *sdev)
{
	struct b43_wl *wl = ssb_get_devtypedata(sdev);
	struct b43_wldev *wldev = ssb_get_drvdata(sdev);
	struct b43_bus_dev *dev = wldev->dev;

	/* We must cancel any work here before unregistering from ieee80211,
	 * as the ieee80211 unreg will destroy the workqueue. */
	cancel_work_sync(&wldev->restart_work);
	cancel_work_sync(&wl->firmware_load);

	B43_WARN_ON(!wl);
	if (!wldev->fw.ucode.data)
		return;			/* NULL if firmware never loaded */
	if (wl->current_dev == wldev && wl->hw_registred) {
		b43_leds_stop(wldev);
		ieee80211_unregister_hw(wl->hw);
	}

	b43_one_core_detach(dev);

	if (list_empty(&wl->devlist)) {
		b43_leds_unregister(wl);
		/* Last core on the chip unregistered.
		 * We can destroy common struct b43_wl.
		 */
		b43_wireless_exit(dev, wl);
	}
}