static void b43_upload_beacon0(struct b43_wldev *dev)
{
	struct b43_wl *wl = dev->wl;

	if (wl->beacon0_uploaded)
		return;
	b43_write_beacon_template(dev, B43_SHM_SH_BT_BASE0, B43_SHM_SH_BTL0);
	wl->beacon0_uploaded = true;
}