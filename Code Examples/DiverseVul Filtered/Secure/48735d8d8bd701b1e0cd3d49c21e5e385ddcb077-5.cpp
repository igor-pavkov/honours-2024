static int lbs_do_scan(struct lbs_private *priv, uint8_t bsstype,
		       struct chanscanparamset *chan_list, int chan_count)
{
	int ret = -ENOMEM;
	struct cmd_ds_802_11_scan *scan_cmd;
	uint8_t *tlv;	/* pointer into our current, growing TLV storage area */

	lbs_deb_enter_args(LBS_DEB_SCAN, "bsstype %d, chanlist[].chan %d, chan_count %d",
		bsstype, chan_list ? chan_list[0].channumber : -1,
		chan_count);

	/* create the fixed part for scan command */
	scan_cmd = kzalloc(MAX_SCAN_CFG_ALLOC, GFP_KERNEL);
	if (scan_cmd == NULL)
		goto out;

	tlv = scan_cmd->tlvbuffer;
	/* TODO: do we need to scan for a specific BSSID?
	memcpy(scan_cmd->bssid, priv->scan_bssid, ETH_ALEN); */
	scan_cmd->bsstype = bsstype;

	/* add TLVs */
	if (priv->scan_ssid_len)
		tlv += lbs_scan_add_ssid_tlv(priv, tlv);
	if (chan_list && chan_count)
		tlv += lbs_scan_add_chanlist_tlv(tlv, chan_list, chan_count);
	tlv += lbs_scan_add_rates_tlv(tlv);

	/* This is the final data we are about to send */
	scan_cmd->hdr.size = cpu_to_le16(tlv - (uint8_t *)scan_cmd);
	lbs_deb_hex(LBS_DEB_SCAN, "SCAN_CMD", (void *)scan_cmd,
		    sizeof(*scan_cmd));
	lbs_deb_hex(LBS_DEB_SCAN, "SCAN_TLV", scan_cmd->tlvbuffer,
		    tlv - scan_cmd->tlvbuffer);

	ret = __lbs_cmd(priv, CMD_802_11_SCAN, &scan_cmd->hdr,
			le16_to_cpu(scan_cmd->hdr.size),
			lbs_ret_80211_scan, 0);

out:
	kfree(scan_cmd);
	lbs_deb_leave_args(LBS_DEB_SCAN, "ret %d", ret);
	return ret;
}