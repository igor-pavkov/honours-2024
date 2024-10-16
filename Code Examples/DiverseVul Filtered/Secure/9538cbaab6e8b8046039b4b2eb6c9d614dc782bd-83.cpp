static void b43_upload_beacon1(struct b43_wldev *dev)
{
	struct b43_wl *wl = dev->wl;

	if (wl->beacon1_uploaded)
		return;
	b43_write_beacon_template(dev, B43_SHM_SH_BT_BASE1, B43_SHM_SH_BTL1);
	wl->beacon1_uploaded = true;
}