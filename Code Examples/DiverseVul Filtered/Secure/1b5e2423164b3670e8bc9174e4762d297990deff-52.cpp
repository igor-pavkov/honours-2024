static void brcmf_escan_timeout(struct timer_list *t)
{
	struct brcmf_cfg80211_info *cfg =
			from_timer(cfg, t, escan_timeout);
	struct wiphy *wiphy = cfg_to_wiphy(cfg);

	if (cfg->int_escan_map || cfg->scan_request) {
		bphy_err(wiphy, "timer expired\n");
		schedule_work(&cfg->escan_timeout_work);
	}
}