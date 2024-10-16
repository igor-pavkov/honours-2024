void lbs_scan_worker(struct work_struct *work)
{
	struct lbs_private *priv =
		container_of(work, struct lbs_private, scan_work.work);

	lbs_deb_enter(LBS_DEB_SCAN);
	lbs_scan_networks(priv, 0);
	lbs_deb_leave(LBS_DEB_SCAN);
}