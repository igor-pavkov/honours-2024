static int lbs_ret_80211_scan(struct lbs_private *priv, unsigned long dummy,
			      struct cmd_header *resp)
{
	struct cmd_ds_802_11_scan_rsp *scanresp = (void *)resp;
	struct bss_descriptor *iter_bss;
	struct bss_descriptor *safe;
	uint8_t *bssinfo;
	uint16_t scanrespsize;
	int bytesleft;
	int idx;
	int tlvbufsize;
	int ret;

	lbs_deb_enter(LBS_DEB_SCAN);

	/* Prune old entries from scan table */
	list_for_each_entry_safe (iter_bss, safe, &priv->network_list, list) {
		unsigned long stale_time = iter_bss->last_scanned + DEFAULT_MAX_SCAN_AGE;
		if (time_before(jiffies, stale_time))
			continue;
		list_move_tail (&iter_bss->list, &priv->network_free_list);
		clear_bss_descriptor(iter_bss);
	}

	if (scanresp->nr_sets > MAX_NETWORK_COUNT) {
		lbs_deb_scan("SCAN_RESP: too many scan results (%d, max %d)\n",
			     scanresp->nr_sets, MAX_NETWORK_COUNT);
		ret = -1;
		goto done;
	}

	bytesleft = le16_to_cpu(scanresp->bssdescriptsize);
	lbs_deb_scan("SCAN_RESP: bssdescriptsize %d\n", bytesleft);

	scanrespsize = le16_to_cpu(resp->size);
	lbs_deb_scan("SCAN_RESP: scan results %d\n", scanresp->nr_sets);

	bssinfo = scanresp->bssdesc_and_tlvbuffer;

	/* The size of the TLV buffer is equal to the entire command response
	 *   size (scanrespsize) minus the fixed fields (sizeof()'s), the
	 *   BSS Descriptions (bssdescriptsize as bytesLef) and the command
	 *   response header (S_DS_GEN)
	 */
	tlvbufsize = scanrespsize - (bytesleft + sizeof(scanresp->bssdescriptsize)
				     + sizeof(scanresp->nr_sets)
				     + S_DS_GEN);

	/*
	 *  Process each scan response returned (scanresp->nr_sets). Save
	 *    the information in the newbssentry and then insert into the
	 *    driver scan table either as an update to an existing entry
	 *    or as an addition at the end of the table
	 */
	for (idx = 0; idx < scanresp->nr_sets && bytesleft; idx++) {
		struct bss_descriptor new;
		struct bss_descriptor *found = NULL;
		struct bss_descriptor *oldest = NULL;
		DECLARE_MAC_BUF(mac);

		/* Process the data fields and IEs returned for this BSS */
		memset(&new, 0, sizeof (struct bss_descriptor));
		if (lbs_process_bss(&new, &bssinfo, &bytesleft) != 0) {
			/* error parsing the scan response, skipped */
			lbs_deb_scan("SCAN_RESP: process_bss returned ERROR\n");
			continue;
		}

		/* Try to find this bss in the scan table */
		list_for_each_entry (iter_bss, &priv->network_list, list) {
			if (is_same_network(iter_bss, &new)) {
				found = iter_bss;
				break;
			}

			if ((oldest == NULL) ||
			    (iter_bss->last_scanned < oldest->last_scanned))
				oldest = iter_bss;
		}

		if (found) {
			/* found, clear it */
			clear_bss_descriptor(found);
		} else if (!list_empty(&priv->network_free_list)) {
			/* Pull one from the free list */
			found = list_entry(priv->network_free_list.next,
					   struct bss_descriptor, list);
			list_move_tail(&found->list, &priv->network_list);
		} else if (oldest) {
			/* If there are no more slots, expire the oldest */
			found = oldest;
			clear_bss_descriptor(found);
			list_move_tail(&found->list, &priv->network_list);
		} else {
			continue;
		}

		lbs_deb_scan("SCAN_RESP: BSSID %s\n", print_mac(mac, new.bssid));

		/* Copy the locally created newbssentry to the scan table */
		memcpy(found, &new, offsetof(struct bss_descriptor, list));
	}

	ret = 0;

done:
	lbs_deb_leave_args(LBS_DEB_SCAN, "ret %d", ret);
	return ret;
}