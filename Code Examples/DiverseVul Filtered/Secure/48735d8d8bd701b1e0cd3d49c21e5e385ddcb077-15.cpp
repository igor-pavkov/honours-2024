int lbs_get_scan(struct net_device *dev, struct iw_request_info *info,
		 struct iw_point *dwrq, char *extra)
{
#define SCAN_ITEM_SIZE 128
	struct lbs_private *priv = dev->priv;
	int err = 0;
	char *ev = extra;
	char *stop = ev + dwrq->length;
	struct bss_descriptor *iter_bss;
	struct bss_descriptor *safe;

	lbs_deb_enter(LBS_DEB_WEXT);

	/* iwlist should wait until the current scan is finished */
	if (priv->scan_channel)
		return -EAGAIN;

	/* Update RSSI if current BSS is a locally created ad-hoc BSS */
	if ((priv->mode == IW_MODE_ADHOC) && priv->adhoccreate)
		lbs_prepare_and_send_command(priv, CMD_802_11_RSSI, 0,
					     CMD_OPTION_WAITFORRSP, 0, NULL);

	mutex_lock(&priv->lock);
	list_for_each_entry_safe (iter_bss, safe, &priv->network_list, list) {
		char *next_ev;
		unsigned long stale_time;

		if (stop - ev < SCAN_ITEM_SIZE) {
			err = -E2BIG;
			break;
		}

		/* For mesh device, list only mesh networks */
		if (dev == priv->mesh_dev && !iter_bss->mesh)
			continue;

		/* Prune old an old scan result */
		stale_time = iter_bss->last_scanned + DEFAULT_MAX_SCAN_AGE;
		if (time_after(jiffies, stale_time)) {
			list_move_tail(&iter_bss->list, &priv->network_free_list);
			clear_bss_descriptor(iter_bss);
			continue;
		}

		/* Translate to WE format this entry */
		next_ev = lbs_translate_scan(priv, info, ev, stop, iter_bss);
		if (next_ev == NULL)
			continue;
		ev = next_ev;
	}
	mutex_unlock(&priv->lock);

	dwrq->length = (ev - extra);
	dwrq->flags = 0;

	lbs_deb_leave_args(LBS_DEB_WEXT, "ret %d", err);
	return err;
}