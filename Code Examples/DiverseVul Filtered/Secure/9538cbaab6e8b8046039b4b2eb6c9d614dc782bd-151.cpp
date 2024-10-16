static void b43_op_stop(struct ieee80211_hw *hw)
{
	struct b43_wl *wl = hw_to_b43_wl(hw);
	struct b43_wldev *dev = wl->current_dev;

	cancel_work_sync(&(wl->beacon_update_trigger));

	if (!dev)
		goto out;

	mutex_lock(&wl->mutex);
	if (b43_status(dev) >= B43_STAT_STARTED) {
		dev = b43_wireless_core_stop(dev);
		if (!dev)
			goto out_unlock;
	}
	b43_wireless_core_exit(dev);
	wl->radio_enabled = false;

out_unlock:
	mutex_unlock(&wl->mutex);
out:
	cancel_work_sync(&(wl->txpower_adjust_work));
}