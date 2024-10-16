int lbs_send_specific_ssid_scan(struct lbs_private *priv, uint8_t *ssid,
				uint8_t ssid_len)
{
	int ret = 0;

	lbs_deb_enter_args(LBS_DEB_SCAN, "SSID '%s'\n",
			   escape_essid(ssid, ssid_len));

	if (!ssid_len)
		goto out;

	memcpy(priv->scan_ssid, ssid, ssid_len);
	priv->scan_ssid_len = ssid_len;

	lbs_scan_networks(priv, 1);
	if (priv->surpriseremoved) {
		ret = -1;
		goto out;
	}

out:
	lbs_deb_leave_args(LBS_DEB_SCAN, "ret %d", ret);
	return ret;
}