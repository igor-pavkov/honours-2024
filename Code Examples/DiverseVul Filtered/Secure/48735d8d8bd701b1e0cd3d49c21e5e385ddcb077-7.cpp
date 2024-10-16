static int lbs_scan_create_channel_list(struct lbs_private *priv,
					struct chanscanparamset *scanchanlist)
{
	struct region_channel *scanregion;
	struct chan_freq_power *cfp;
	int rgnidx;
	int chanidx;
	int nextchan;
	uint8_t scantype;

	chanidx = 0;

	/* Set the default scan type to the user specified type, will later
	 *   be changed to passive on a per channel basis if restricted by
	 *   regulatory requirements (11d or 11h)
	 */
	scantype = CMD_SCAN_TYPE_ACTIVE;

	for (rgnidx = 0; rgnidx < ARRAY_SIZE(priv->region_channel); rgnidx++) {
		if (priv->enable11d && (priv->connect_status != LBS_CONNECTED)
		    && (priv->mesh_connect_status != LBS_CONNECTED)) {
			/* Scan all the supported chan for the first scan */
			if (!priv->universal_channel[rgnidx].valid)
				continue;
			scanregion = &priv->universal_channel[rgnidx];

			/* clear the parsed_region_chan for the first scan */
			memset(&priv->parsed_region_chan, 0x00,
			       sizeof(priv->parsed_region_chan));
		} else {
			if (!priv->region_channel[rgnidx].valid)
				continue;
			scanregion = &priv->region_channel[rgnidx];
		}

		for (nextchan = 0; nextchan < scanregion->nrcfp; nextchan++, chanidx++) {
			struct chanscanparamset *chan = &scanchanlist[chanidx];

			cfp = scanregion->CFP + nextchan;

			if (priv->enable11d)
				scantype = lbs_get_scan_type_11d(cfp->channel,
								 &priv->parsed_region_chan);

			if (scanregion->band == BAND_B || scanregion->band == BAND_G)
				chan->radiotype = CMD_SCAN_RADIO_TYPE_BG;

			if (scantype == CMD_SCAN_TYPE_PASSIVE) {
				chan->maxscantime = cpu_to_le16(MRVDRV_PASSIVE_SCAN_CHAN_TIME);
				chan->chanscanmode.passivescan = 1;
			} else {
				chan->maxscantime = cpu_to_le16(MRVDRV_ACTIVE_SCAN_CHAN_TIME);
				chan->chanscanmode.passivescan = 0;
			}

			chan->channumber = cfp->channel;
		}
	}
	return chanidx;
}