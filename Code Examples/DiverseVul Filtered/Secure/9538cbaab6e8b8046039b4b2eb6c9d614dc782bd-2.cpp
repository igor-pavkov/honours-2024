static int b43_bcma_probe(struct bcma_device *core)
{
	struct b43_bus_dev *dev;
	struct b43_wl *wl;
	int err;

	dev = b43_bus_dev_bcma_init(core);
	if (!dev)
		return -ENODEV;

	wl = b43_wireless_init(dev);
	if (IS_ERR(wl)) {
		err = PTR_ERR(wl);
		goto bcma_out;
	}

	err = b43_one_core_attach(dev, wl);
	if (err)
		goto bcma_err_wireless_exit;

	/* setup and start work to load firmware */
	INIT_WORK(&wl->firmware_load, b43_request_firmware);
	schedule_work(&wl->firmware_load);

bcma_out:
	return err;

bcma_err_wireless_exit:
	ieee80211_free_hw(wl->hw);
	return err;
}