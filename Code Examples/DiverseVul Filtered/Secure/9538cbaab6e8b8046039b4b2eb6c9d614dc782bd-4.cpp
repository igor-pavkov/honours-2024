static void b43_op_update_tkip_key(struct ieee80211_hw *hw,
				   struct ieee80211_vif *vif,
				   struct ieee80211_key_conf *keyconf,
				   struct ieee80211_sta *sta,
				   u32 iv32, u16 *phase1key)
{
	struct b43_wl *wl = hw_to_b43_wl(hw);
	struct b43_wldev *dev;
	int index = keyconf->hw_key_idx;

	if (B43_WARN_ON(!modparam_hwtkip))
		return;

	/* This is only called from the RX path through mac80211, where
	 * our mutex is already locked. */
	B43_WARN_ON(!mutex_is_locked(&wl->mutex));
	dev = wl->current_dev;
	B43_WARN_ON(!dev || b43_status(dev) < B43_STAT_INITIALIZED);

	keymac_write(dev, index, NULL);	/* First zero out mac to avoid race */

	rx_tkip_phase1_write(dev, index, iv32, phase1key);
	/* only pairwise TKIP keys are supported right now */
	if (WARN_ON(!sta))
		return;
	keymac_write(dev, index, sta->addr);
}