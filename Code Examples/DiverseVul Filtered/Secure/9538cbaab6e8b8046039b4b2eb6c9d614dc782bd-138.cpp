static void b43_op_remove_interface(struct ieee80211_hw *hw,
				    struct ieee80211_vif *vif)
{
	struct b43_wl *wl = hw_to_b43_wl(hw);
	struct b43_wldev *dev = wl->current_dev;

	b43dbg(wl, "Removing Interface type %d\n", vif->type);

	mutex_lock(&wl->mutex);

	B43_WARN_ON(!wl->operating);
	B43_WARN_ON(wl->vif != vif);
	wl->vif = NULL;

	wl->operating = false;

	b43_adjust_opmode(dev);
	memset(wl->mac_addr, 0, ETH_ALEN);
	b43_upload_card_macaddress(dev);

	mutex_unlock(&wl->mutex);
}