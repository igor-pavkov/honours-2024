static inline char *lbs_translate_scan(struct lbs_private *priv,
					    struct iw_request_info *info,
					    char *start, char *stop,
					    struct bss_descriptor *bss)
{
	struct chan_freq_power *cfp;
	char *current_val;	/* For rates */
	struct iw_event iwe;	/* Temporary buffer */
	int j;
#define PERFECT_RSSI ((uint8_t)50)
#define WORST_RSSI   ((uint8_t)0)
#define RSSI_DIFF    ((uint8_t)(PERFECT_RSSI - WORST_RSSI))
	uint8_t rssi;

	lbs_deb_enter(LBS_DEB_SCAN);

	cfp = lbs_find_cfp_by_band_and_channel(priv, 0, bss->channel);
	if (!cfp) {
		lbs_deb_scan("Invalid channel number %d\n", bss->channel);
		start = NULL;
		goto out;
	}

	/* First entry *MUST* be the BSSID */
	iwe.cmd = SIOCGIWAP;
	iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
	memcpy(iwe.u.ap_addr.sa_data, &bss->bssid, ETH_ALEN);
	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_ADDR_LEN);

	/* SSID */
	iwe.cmd = SIOCGIWESSID;
	iwe.u.data.flags = 1;
	iwe.u.data.length = min((uint32_t) bss->ssid_len, (uint32_t) IW_ESSID_MAX_SIZE);
	start = iwe_stream_add_point(info, start, stop, &iwe, bss->ssid);

	/* Mode */
	iwe.cmd = SIOCGIWMODE;
	iwe.u.mode = bss->mode;
	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_UINT_LEN);

	/* Frequency */
	iwe.cmd = SIOCGIWFREQ;
	iwe.u.freq.m = (long)cfp->freq * 100000;
	iwe.u.freq.e = 1;
	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_FREQ_LEN);

	/* Add quality statistics */
	iwe.cmd = IWEVQUAL;
	iwe.u.qual.updated = IW_QUAL_ALL_UPDATED;
	iwe.u.qual.level = SCAN_RSSI(bss->rssi);

	rssi = iwe.u.qual.level - MRVDRV_NF_DEFAULT_SCAN_VALUE;
	iwe.u.qual.qual =
		(100 * RSSI_DIFF * RSSI_DIFF - (PERFECT_RSSI - rssi) *
		 (15 * (RSSI_DIFF) + 62 * (PERFECT_RSSI - rssi))) /
		(RSSI_DIFF * RSSI_DIFF);
	if (iwe.u.qual.qual > 100)
		iwe.u.qual.qual = 100;

	if (priv->NF[TYPE_BEACON][TYPE_NOAVG] == 0) {
		iwe.u.qual.noise = MRVDRV_NF_DEFAULT_SCAN_VALUE;
	} else {
		iwe.u.qual.noise = CAL_NF(priv->NF[TYPE_BEACON][TYPE_NOAVG]);
	}

	/* Locally created ad-hoc BSSs won't have beacons if this is the
	 * only station in the adhoc network; so get signal strength
	 * from receive statistics.
	 */
	if ((priv->mode == IW_MODE_ADHOC) && priv->adhoccreate
	    && !lbs_ssid_cmp(priv->curbssparams.ssid,
			     priv->curbssparams.ssid_len,
			     bss->ssid, bss->ssid_len)) {
		int snr, nf;
		snr = priv->SNR[TYPE_RXPD][TYPE_AVG] / AVG_SCALE;
		nf = priv->NF[TYPE_RXPD][TYPE_AVG] / AVG_SCALE;
		iwe.u.qual.level = CAL_RSSI(snr, nf);
	}
	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_QUAL_LEN);

	/* Add encryption capability */
	iwe.cmd = SIOCGIWENCODE;
	if (bss->capability & WLAN_CAPABILITY_PRIVACY) {
		iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
	} else {
		iwe.u.data.flags = IW_ENCODE_DISABLED;
	}
	iwe.u.data.length = 0;
	start = iwe_stream_add_point(info, start, stop, &iwe, bss->ssid);

	current_val = start + iwe_stream_lcp_len(info);

	iwe.cmd = SIOCGIWRATE;
	iwe.u.bitrate.fixed = 0;
	iwe.u.bitrate.disabled = 0;
	iwe.u.bitrate.value = 0;

	for (j = 0; bss->rates[j] && (j < sizeof(bss->rates)); j++) {
		/* Bit rate given in 500 kb/s units */
		iwe.u.bitrate.value = bss->rates[j] * 500000;
		current_val = iwe_stream_add_value(info, start, current_val,
						   stop, &iwe, IW_EV_PARAM_LEN);
	}
	if ((bss->mode == IW_MODE_ADHOC) && priv->adhoccreate
	    && !lbs_ssid_cmp(priv->curbssparams.ssid,
			     priv->curbssparams.ssid_len,
			     bss->ssid, bss->ssid_len)) {
		iwe.u.bitrate.value = 22 * 500000;
		current_val = iwe_stream_add_value(info, start, current_val,
						   stop, &iwe, IW_EV_PARAM_LEN);
	}
	/* Check if we added any event */
	if ((current_val - start) > iwe_stream_lcp_len(info))
		start = current_val;

	memset(&iwe, 0, sizeof(iwe));
	if (bss->wpa_ie_len) {
		char buf[MAX_WPA_IE_LEN];
		memcpy(buf, bss->wpa_ie, bss->wpa_ie_len);
		iwe.cmd = IWEVGENIE;
		iwe.u.data.length = bss->wpa_ie_len;
		start = iwe_stream_add_point(info, start, stop, &iwe, buf);
	}

	memset(&iwe, 0, sizeof(iwe));
	if (bss->rsn_ie_len) {
		char buf[MAX_WPA_IE_LEN];
		memcpy(buf, bss->rsn_ie, bss->rsn_ie_len);
		iwe.cmd = IWEVGENIE;
		iwe.u.data.length = bss->rsn_ie_len;
		start = iwe_stream_add_point(info, start, stop, &iwe, buf);
	}

	if (bss->mesh) {
		char custom[MAX_CUSTOM_LEN];
		char *p = custom;

		iwe.cmd = IWEVCUSTOM;
		p += snprintf(p, MAX_CUSTOM_LEN, "mesh-type: olpc");
		iwe.u.data.length = p - custom;
		if (iwe.u.data.length)
			start = iwe_stream_add_point(info, start, stop,
						     &iwe, custom);
	}

out:
	lbs_deb_leave_args(LBS_DEB_SCAN, "start %p", start);
	return start;
}