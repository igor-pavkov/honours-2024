static s32 brcmf_inform_single_bss(struct brcmf_cfg80211_info *cfg,
				   struct brcmf_bss_info_le *bi)
{
	struct wiphy *wiphy = cfg_to_wiphy(cfg);
	struct cfg80211_bss *bss;
	enum nl80211_band band;
	struct brcmu_chan ch;
	u16 channel;
	u32 freq;
	u16 notify_capability;
	u16 notify_interval;
	u8 *notify_ie;
	size_t notify_ielen;
	struct cfg80211_inform_bss bss_data = {};

	if (le32_to_cpu(bi->length) > WL_BSS_INFO_MAX) {
		bphy_err(wiphy, "Bss info is larger than buffer. Discarding\n");
		return 0;
	}

	if (!bi->ctl_ch) {
		ch.chspec = le16_to_cpu(bi->chanspec);
		cfg->d11inf.decchspec(&ch);
		bi->ctl_ch = ch.control_ch_num;
	}
	channel = bi->ctl_ch;

	if (channel <= CH_MAX_2G_CHANNEL)
		band = NL80211_BAND_2GHZ;
	else
		band = NL80211_BAND_5GHZ;

	freq = ieee80211_channel_to_frequency(channel, band);
	bss_data.chan = ieee80211_get_channel(wiphy, freq);
	bss_data.scan_width = NL80211_BSS_CHAN_WIDTH_20;
	bss_data.boottime_ns = ktime_to_ns(ktime_get_boottime());

	notify_capability = le16_to_cpu(bi->capability);
	notify_interval = le16_to_cpu(bi->beacon_period);
	notify_ie = (u8 *)bi + le16_to_cpu(bi->ie_offset);
	notify_ielen = le32_to_cpu(bi->ie_length);
	bss_data.signal = (s16)le16_to_cpu(bi->RSSI) * 100;

	brcmf_dbg(CONN, "bssid: %pM\n", bi->BSSID);
	brcmf_dbg(CONN, "Channel: %d(%d)\n", channel, freq);
	brcmf_dbg(CONN, "Capability: %X\n", notify_capability);
	brcmf_dbg(CONN, "Beacon interval: %d\n", notify_interval);
	brcmf_dbg(CONN, "Signal: %d\n", bss_data.signal);

	bss = cfg80211_inform_bss_data(wiphy, &bss_data,
				       CFG80211_BSS_FTYPE_UNKNOWN,
				       (const u8 *)bi->BSSID,
				       0, notify_capability,
				       notify_interval, notify_ie,
				       notify_ielen, GFP_KERNEL);

	if (!bss)
		return -ENOMEM;

	cfg80211_put_bss(wiphy, bss);

	return 0;
}