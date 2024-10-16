static void b43_op_sta_notify(struct ieee80211_hw *hw,
			      struct ieee80211_vif *vif,
			      enum sta_notify_cmd notify_cmd,
			      struct ieee80211_sta *sta)
{
	struct b43_wl *wl = hw_to_b43_wl(hw);

	B43_WARN_ON(!vif || wl->vif != vif);
}