static int b43_switch_band(struct b43_wl *wl, struct ieee80211_channel *chan)
{
	struct b43_wldev *up_dev = NULL;
	struct b43_wldev *down_dev;
	struct b43_wldev *d;
	int err;
	bool uninitialized_var(gmode);
	int prev_status;

	/* Find a device and PHY which supports the band. */
	list_for_each_entry(d, &wl->devlist, list) {
		switch (chan->band) {
		case IEEE80211_BAND_5GHZ:
			if (d->phy.supports_5ghz) {
				up_dev = d;
				gmode = false;
			}
			break;
		case IEEE80211_BAND_2GHZ:
			if (d->phy.supports_2ghz) {
				up_dev = d;
				gmode = true;
			}
			break;
		default:
			B43_WARN_ON(1);
			return -EINVAL;
		}
		if (up_dev)
			break;
	}
	if (!up_dev) {
		b43err(wl, "Could not find a device for %s-GHz band operation\n",
		       band_to_string(chan->band));
		return -ENODEV;
	}
	if ((up_dev == wl->current_dev) &&
	    (!!wl->current_dev->phy.gmode == !!gmode)) {
		/* This device is already running. */
		return 0;
	}
	b43dbg(wl, "Switching to %s-GHz band\n",
	       band_to_string(chan->band));
	down_dev = wl->current_dev;

	prev_status = b43_status(down_dev);
	/* Shutdown the currently running core. */
	if (prev_status >= B43_STAT_STARTED)
		down_dev = b43_wireless_core_stop(down_dev);
	if (prev_status >= B43_STAT_INITIALIZED)
		b43_wireless_core_exit(down_dev);

	if (down_dev != up_dev) {
		/* We switch to a different core, so we put PHY into
		 * RESET on the old core. */
		b43_put_phy_into_reset(down_dev);
	}

	/* Now start the new core. */
	up_dev->phy.gmode = gmode;
	if (prev_status >= B43_STAT_INITIALIZED) {
		err = b43_wireless_core_init(up_dev);
		if (err) {
			b43err(wl, "Fatal: Could not initialize device for "
			       "selected %s-GHz band\n",
			       band_to_string(chan->band));
			goto init_failure;
		}
	}
	if (prev_status >= B43_STAT_STARTED) {
		err = b43_wireless_core_start(up_dev);
		if (err) {
			b43err(wl, "Fatal: Could not start device for "
			       "selected %s-GHz band\n",
			       band_to_string(chan->band));
			b43_wireless_core_exit(up_dev);
			goto init_failure;
		}
	}
	B43_WARN_ON(b43_status(up_dev) != prev_status);

	wl->current_dev = up_dev;

	return 0;
init_failure:
	/* Whoops, failed to init the new core. No core is operating now. */
	wl->current_dev = NULL;
	return err;
}