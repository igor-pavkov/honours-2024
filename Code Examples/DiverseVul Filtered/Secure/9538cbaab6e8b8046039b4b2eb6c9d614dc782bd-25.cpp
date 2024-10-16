static int b43_op_beacon_set_tim(struct ieee80211_hw *hw,
				 struct ieee80211_sta *sta, bool set)
{
	struct b43_wl *wl = hw_to_b43_wl(hw);

	/* FIXME: add locking */
	b43_update_templates(wl);

	return 0;
}