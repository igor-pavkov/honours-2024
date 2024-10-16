int b43_ssb_probe(struct ssb_device *sdev, const struct ssb_device_id *id)
{
	struct b43_bus_dev *dev;
	struct b43_wl *wl;
	int err;
	int first = 0;

	dev = b43_bus_dev_ssb_init(sdev);
	if (!dev)
		return -ENOMEM;

	wl = ssb_get_devtypedata(sdev);
	if (!wl) {
		/* Probing the first core. Must setup common struct b43_wl */
		first = 1;
		b43_sprom_fixup(sdev->bus);
		wl = b43_wireless_init(dev);
		if (IS_ERR(wl)) {
			err = PTR_ERR(wl);
			goto out;
		}
		ssb_set_devtypedata(sdev, wl);
		B43_WARN_ON(ssb_get_devtypedata(sdev) != wl);
	}
	err = b43_one_core_attach(dev, wl);
	if (err)
		goto err_wireless_exit;

	/* setup and start work to load firmware */
	INIT_WORK(&wl->firmware_load, b43_request_firmware);
	schedule_work(&wl->firmware_load);

      out:
	return err;

      err_wireless_exit:
	if (first)
		b43_wireless_exit(dev, wl);
	return err;
}