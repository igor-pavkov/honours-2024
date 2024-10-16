static void b43_op_sw_scan_start_notifier(struct ieee80211_hw *hw)
{
	struct b43_wl *wl = hw_to_b43_wl(hw);
	struct b43_wldev *dev;

	mutex_lock(&wl->mutex);
	dev = wl->current_dev;
	if (dev && (b43_status(dev) >= B43_STAT_INITIALIZED)) {
		/* Disable CFP update during scan on other channels. */
		b43_hf_write(dev, b43_hf_read(dev) | B43_HF_SKCFPUP);
	}
	mutex_unlock(&wl->mutex);
}