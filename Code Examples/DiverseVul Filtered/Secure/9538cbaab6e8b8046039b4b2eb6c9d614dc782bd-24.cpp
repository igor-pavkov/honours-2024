static int b43_op_config(struct ieee80211_hw *hw, u32 changed)
{
	struct b43_wl *wl = hw_to_b43_wl(hw);
	struct b43_wldev *dev;
	struct b43_phy *phy;
	struct ieee80211_conf *conf = &hw->conf;
	int antenna;
	int err = 0;
	bool reload_bss = false;

	mutex_lock(&wl->mutex);

	dev = wl->current_dev;

	/* Switch the band (if necessary). This might change the active core. */
	err = b43_switch_band(wl, conf->chandef.chan);
	if (err)
		goto out_unlock_mutex;

	/* Need to reload all settings if the core changed */
	if (dev != wl->current_dev) {
		dev = wl->current_dev;
		changed = ~0;
		reload_bss = true;
	}

	phy = &dev->phy;

	if (conf_is_ht(conf))
		phy->is_40mhz =
			(conf_is_ht40_minus(conf) || conf_is_ht40_plus(conf));
	else
		phy->is_40mhz = false;

	b43_mac_suspend(dev);

	if (changed & IEEE80211_CONF_CHANGE_RETRY_LIMITS)
		b43_set_retry_limits(dev, conf->short_frame_max_tx_count,
					  conf->long_frame_max_tx_count);
	changed &= ~IEEE80211_CONF_CHANGE_RETRY_LIMITS;
	if (!changed)
		goto out_mac_enable;

	/* Switch to the requested channel.
	 * The firmware takes care of races with the TX handler. */
	if (conf->chandef.chan->hw_value != phy->channel)
		b43_switch_channel(dev, conf->chandef.chan->hw_value);

	dev->wl->radiotap_enabled = !!(conf->flags & IEEE80211_CONF_MONITOR);

	/* Adjust the desired TX power level. */
	if (conf->power_level != 0) {
		if (conf->power_level != phy->desired_txpower) {
			phy->desired_txpower = conf->power_level;
			b43_phy_txpower_check(dev, B43_TXPWR_IGNORE_TIME |
						   B43_TXPWR_IGNORE_TSSI);
		}
	}

	/* Antennas for RX and management frame TX. */
	antenna = B43_ANTENNA_DEFAULT;
	b43_mgmtframe_txantenna(dev, antenna);
	antenna = B43_ANTENNA_DEFAULT;
	if (phy->ops->set_rx_antenna)
		phy->ops->set_rx_antenna(dev, antenna);

	if (wl->radio_enabled != phy->radio_on) {
		if (wl->radio_enabled) {
			b43_software_rfkill(dev, false);
			b43info(dev->wl, "Radio turned on by software\n");
			if (!dev->radio_hw_enable) {
				b43info(dev->wl, "The hardware RF-kill button "
					"still turns the radio physically off. "
					"Press the button to turn it on.\n");
			}
		} else {
			b43_software_rfkill(dev, true);
			b43info(dev->wl, "Radio turned off by software\n");
		}
	}

out_mac_enable:
	b43_mac_enable(dev);
out_unlock_mutex:
	mutex_unlock(&wl->mutex);

	if (wl->vif && reload_bss)
		b43_op_bss_info_changed(hw, wl->vif, &wl->vif->bss_conf, ~0);

	return err;
}