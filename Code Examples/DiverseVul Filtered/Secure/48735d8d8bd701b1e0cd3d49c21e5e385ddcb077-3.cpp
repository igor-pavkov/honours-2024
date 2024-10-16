int lbs_set_scan(struct net_device *dev, struct iw_request_info *info,
		 union iwreq_data *wrqu, char *extra)
{
	struct lbs_private *priv = dev->priv;
	int ret = 0;

	lbs_deb_enter(LBS_DEB_WEXT);

	if (!priv->radio_on) {
		ret = -EINVAL;
		goto out;
	}

	if (!netif_running(dev)) {
		ret = -ENETDOWN;
		goto out;
	}

	/* mac80211 does this:
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type != IEEE80211_IF_TYPE_xxx) {
		ret = -EOPNOTSUPP;
		goto out;
	}
	*/

	if (wrqu->data.length == sizeof(struct iw_scan_req) &&
	    wrqu->data.flags & IW_SCAN_THIS_ESSID) {
		struct iw_scan_req *req = (struct iw_scan_req *)extra;
		priv->scan_ssid_len = req->essid_len;
		memcpy(priv->scan_ssid, req->essid, priv->scan_ssid_len);
		lbs_deb_wext("set_scan, essid '%s'\n",
			escape_essid(priv->scan_ssid, priv->scan_ssid_len));
	} else {
		priv->scan_ssid_len = 0;
	}

	if (!delayed_work_pending(&priv->scan_work))
		queue_delayed_work(priv->work_thread, &priv->scan_work,
				   msecs_to_jiffies(50));
	/* set marker that currently a scan is taking place */
	priv->scan_channel = -1;

	if (priv->surpriseremoved)
		ret = -EIO;

out:
	lbs_deb_leave_args(LBS_DEB_WEXT, "ret %d", ret);
	return ret;
}