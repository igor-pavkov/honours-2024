static void b43_wireless_exit(struct b43_bus_dev *dev, struct b43_wl *wl)
{
	struct ieee80211_hw *hw = wl->hw;

	ssb_set_devtypedata(dev->sdev, NULL);
	ieee80211_free_hw(hw);
}