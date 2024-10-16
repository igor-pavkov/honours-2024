static void b43_one_core_detach(struct b43_bus_dev *dev)
{
	struct b43_wldev *wldev;
	struct b43_wl *wl;

	/* Do not cancel ieee80211-workqueue based work here.
	 * See comment in b43_remove(). */

	wldev = b43_bus_get_wldev(dev);
	wl = wldev->wl;
	b43_debugfs_remove_device(wldev);
	b43_wireless_core_detach(wldev);
	list_del(&wldev->list);
	wl->nr_devs--;
	b43_bus_set_wldev(dev, NULL);
	kfree(wldev);
}