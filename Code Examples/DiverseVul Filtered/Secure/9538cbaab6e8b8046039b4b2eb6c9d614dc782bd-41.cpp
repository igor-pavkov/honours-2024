static int b43_wireless_core_attach(struct b43_wldev *dev)
{
	struct b43_wl *wl = dev->wl;
	struct pci_dev *pdev = NULL;
	int err;
	u32 tmp;
	bool have_2ghz_phy = false, have_5ghz_phy = false;

	/* Do NOT do any device initialization here.
	 * Do it in wireless_core_init() instead.
	 * This function is for gathering basic information about the HW, only.
	 * Also some structs may be set up here. But most likely you want to have
	 * that in core_init(), too.
	 */

#ifdef CONFIG_B43_SSB
	if (dev->dev->bus_type == B43_BUS_SSB &&
	    dev->dev->sdev->bus->bustype == SSB_BUSTYPE_PCI)
		pdev = dev->dev->sdev->bus->host_pci;
#endif

	err = b43_bus_powerup(dev, 0);
	if (err) {
		b43err(wl, "Bus powerup failed\n");
		goto out;
	}

	/* Get the PHY type. */
	switch (dev->dev->bus_type) {
#ifdef CONFIG_B43_BCMA
	case B43_BUS_BCMA:
		tmp = bcma_aread32(dev->dev->bdev, BCMA_IOST);
		have_2ghz_phy = !!(tmp & B43_BCMA_IOST_2G_PHY);
		have_5ghz_phy = !!(tmp & B43_BCMA_IOST_5G_PHY);
		break;
#endif
#ifdef CONFIG_B43_SSB
	case B43_BUS_SSB:
		if (dev->dev->core_rev >= 5) {
			tmp = ssb_read32(dev->dev->sdev, SSB_TMSHIGH);
			have_2ghz_phy = !!(tmp & B43_TMSHIGH_HAVE_2GHZ_PHY);
			have_5ghz_phy = !!(tmp & B43_TMSHIGH_HAVE_5GHZ_PHY);
		} else
			B43_WARN_ON(1);
		break;
#endif
	}

	dev->phy.gmode = have_2ghz_phy;
	dev->phy.radio_on = true;
	b43_wireless_core_reset(dev, dev->phy.gmode);

	err = b43_phy_versioning(dev);
	if (err)
		goto err_powerdown;
	/* Check if this device supports multiband. */
	if (!pdev ||
	    (pdev->device != 0x4312 &&
	     pdev->device != 0x4319 && pdev->device != 0x4324)) {
		/* No multiband support. */
		have_2ghz_phy = false;
		have_5ghz_phy = false;
		switch (dev->phy.type) {
		case B43_PHYTYPE_A:
			have_5ghz_phy = true;
			break;
		case B43_PHYTYPE_LP: //FIXME not always!
#if 0 //FIXME enabling 5GHz causes a NULL pointer dereference
			have_5ghz_phy = 1;
#endif
		case B43_PHYTYPE_G:
		case B43_PHYTYPE_N:
		case B43_PHYTYPE_HT:
		case B43_PHYTYPE_LCN:
			have_2ghz_phy = true;
			break;
		default:
			B43_WARN_ON(1);
		}
	}
	if (dev->phy.type == B43_PHYTYPE_A) {
		/* FIXME */
		b43err(wl, "IEEE 802.11a devices are unsupported\n");
		err = -EOPNOTSUPP;
		goto err_powerdown;
	}
	if (1 /* disable A-PHY */) {
		/* FIXME: For now we disable the A-PHY on multi-PHY devices. */
		if (dev->phy.type != B43_PHYTYPE_N &&
		    dev->phy.type != B43_PHYTYPE_LP) {
			have_2ghz_phy = true;
			have_5ghz_phy = false;
		}
	}

	err = b43_phy_allocate(dev);
	if (err)
		goto err_powerdown;

	dev->phy.gmode = have_2ghz_phy;
	b43_wireless_core_reset(dev, dev->phy.gmode);

	err = b43_validate_chipaccess(dev);
	if (err)
		goto err_phy_free;
	err = b43_setup_bands(dev, have_2ghz_phy, have_5ghz_phy);
	if (err)
		goto err_phy_free;

	/* Now set some default "current_dev" */
	if (!wl->current_dev)
		wl->current_dev = dev;
	INIT_WORK(&dev->restart_work, b43_chip_reset);

	dev->phy.ops->switch_analog(dev, 0);
	b43_device_disable(dev, 0);
	b43_bus_may_powerdown(dev);

out:
	return err;

err_phy_free:
	b43_phy_free(dev);
err_powerdown:
	b43_bus_may_powerdown(dev);
	return err;
}