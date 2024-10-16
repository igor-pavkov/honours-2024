int lbs_scan_networks(struct lbs_private *priv, int full_scan)
{
	int ret = -ENOMEM;
	struct chanscanparamset *chan_list;
	struct chanscanparamset *curr_chans;
	int chan_count;
	uint8_t bsstype = CMD_BSS_TYPE_ANY;
	int numchannels = MRVDRV_CHANNELS_PER_SCAN_CMD;
	union iwreq_data wrqu;
#ifdef CONFIG_LIBERTAS_DEBUG
	struct bss_descriptor *iter;
	int i = 0;
	DECLARE_MAC_BUF(mac);
#endif

	lbs_deb_enter_args(LBS_DEB_SCAN, "full_scan %d", full_scan);

	/* Cancel any partial outstanding partial scans if this scan
	 * is a full scan.
	 */
	if (full_scan && delayed_work_pending(&priv->scan_work))
		cancel_delayed_work(&priv->scan_work);

	/* User-specified bsstype or channel list
	TODO: this can be implemented if some user-space application
	need the feature. Formerly, it was accessible from debugfs,
	but then nowhere used.
	if (user_cfg) {
		if (user_cfg->bsstype)
		bsstype = user_cfg->bsstype;
	} */

	lbs_deb_scan("numchannels %d, bsstype %d\n", numchannels, bsstype);

	/* Create list of channels to scan */
	chan_list = kzalloc(sizeof(struct chanscanparamset) *
			    LBS_IOCTL_USER_SCAN_CHAN_MAX, GFP_KERNEL);
	if (!chan_list) {
		lbs_pr_alert("SCAN: chan_list empty\n");
		goto out;
	}

	/* We want to scan all channels */
	chan_count = lbs_scan_create_channel_list(priv, chan_list);

	netif_stop_queue(priv->dev);
	netif_carrier_off(priv->dev);
	if (priv->mesh_dev) {
		netif_stop_queue(priv->mesh_dev);
		netif_carrier_off(priv->mesh_dev);
	}

	/* Prepare to continue an interrupted scan */
	lbs_deb_scan("chan_count %d, scan_channel %d\n",
		     chan_count, priv->scan_channel);
	curr_chans = chan_list;
	/* advance channel list by already-scanned-channels */
	if (priv->scan_channel > 0) {
		curr_chans += priv->scan_channel;
		chan_count -= priv->scan_channel;
	}

	/* Send scan command(s)
	 * numchannels contains the number of channels we should maximally scan
	 * chan_count is the total number of channels to scan
	 */

	while (chan_count) {
		int to_scan = min(numchannels, chan_count);
		lbs_deb_scan("scanning %d of %d channels\n",
			     to_scan, chan_count);
		ret = lbs_do_scan(priv, bsstype, curr_chans,
				  to_scan);
		if (ret) {
			lbs_pr_err("SCAN_CMD failed\n");
			goto out2;
		}
		curr_chans += to_scan;
		chan_count -= to_scan;

		/* somehow schedule the next part of the scan */
		if (chan_count && !full_scan &&
		    !priv->surpriseremoved) {
			/* -1 marks just that we're currently scanning */
			if (priv->scan_channel < 0)
				priv->scan_channel = to_scan;
			else
				priv->scan_channel += to_scan;
			cancel_delayed_work(&priv->scan_work);
			queue_delayed_work(priv->work_thread, &priv->scan_work,
					   msecs_to_jiffies(300));
			/* skip over GIWSCAN event */
			goto out;
		}

	}
	memset(&wrqu, 0, sizeof(union iwreq_data));
	wireless_send_event(priv->dev, SIOCGIWSCAN, &wrqu, NULL);

#ifdef CONFIG_LIBERTAS_DEBUG
	/* Dump the scan table */
	mutex_lock(&priv->lock);
	lbs_deb_scan("scan table:\n");
	list_for_each_entry(iter, &priv->network_list, list)
		lbs_deb_scan("%02d: BSSID %s, RSSI %d, SSID '%s'\n",
			     i++, print_mac(mac, iter->bssid), iter->rssi,
			     escape_essid(iter->ssid, iter->ssid_len));
	mutex_unlock(&priv->lock);
#endif

out2:
	priv->scan_channel = 0;

out:
	if (priv->connect_status == LBS_CONNECTED) {
		netif_carrier_on(priv->dev);
		if (!priv->tx_pending_len)
			netif_wake_queue(priv->dev);
	}
	if (priv->mesh_dev && (priv->mesh_connect_status == LBS_CONNECTED)) {
		netif_carrier_on(priv->mesh_dev);
		if (!priv->tx_pending_len)
			netif_wake_queue(priv->mesh_dev);
	}
	kfree(chan_list);

	lbs_deb_leave_args(LBS_DEB_SCAN, "ret %d", ret);
	return ret;
}