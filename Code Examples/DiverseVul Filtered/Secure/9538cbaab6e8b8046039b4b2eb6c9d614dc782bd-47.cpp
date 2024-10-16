static int b43_op_start(struct ieee80211_hw *hw)
{
	struct b43_wl *wl = hw_to_b43_wl(hw);
	struct b43_wldev *dev = wl->current_dev;
	int did_init = 0;
	int err = 0;

	/* Kill all old instance specific information to make sure
	 * the card won't use it in the short timeframe between start
	 * and mac80211 reconfiguring it. */
	memset(wl->bssid, 0, ETH_ALEN);
	memset(wl->mac_addr, 0, ETH_ALEN);
	wl->filter_flags = 0;
	wl->radiotap_enabled = false;
	b43_qos_clear(wl);
	wl->beacon0_uploaded = false;
	wl->beacon1_uploaded = false;
	wl->beacon_templates_virgin = true;
	wl->radio_enabled = true;

	mutex_lock(&wl->mutex);

	if (b43_status(dev) < B43_STAT_INITIALIZED) {
		err = b43_wireless_core_init(dev);
		if (err)
			goto out_mutex_unlock;
		did_init = 1;
	}

	if (b43_status(dev) < B43_STAT_STARTED) {
		err = b43_wireless_core_start(dev);
		if (err) {
			if (did_init)
				b43_wireless_core_exit(dev);
			goto out_mutex_unlock;
		}
	}

	/* XXX: only do if device doesn't support rfkill irq */
	wiphy_rfkill_start_polling(hw->wiphy);

 out_mutex_unlock:
	mutex_unlock(&wl->mutex);

	/*
	 * Configuration may have been overwritten during initialization.
	 * Reload the configuration, but only if initialization was
	 * successful. Reloading the configuration after a failed init
	 * may hang the system.
	 */
	if (!err)
		b43_op_config(hw, ~0);

	return err;
}