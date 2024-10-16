static int b43_wireless_core_init(struct b43_wldev *dev)
{
	struct ssb_sprom *sprom = dev->dev->bus_sprom;
	struct b43_phy *phy = &dev->phy;
	int err;
	u64 hf;

	B43_WARN_ON(b43_status(dev) != B43_STAT_UNINIT);

	err = b43_bus_powerup(dev, 0);
	if (err)
		goto out;
	if (!b43_device_is_enabled(dev))
		b43_wireless_core_reset(dev, phy->gmode);

	/* Reset all data structures. */
	setup_struct_wldev_for_init(dev);
	phy->ops->prepare_structs(dev);

	/* Enable IRQ routing to this device. */
	switch (dev->dev->bus_type) {
#ifdef CONFIG_B43_BCMA
	case B43_BUS_BCMA:
		bcma_core_pci_irq_ctl(&dev->dev->bdev->bus->drv_pci[0],
				      dev->dev->bdev, true);
		break;
#endif
#ifdef CONFIG_B43_SSB
	case B43_BUS_SSB:
		ssb_pcicore_dev_irqvecs_enable(&dev->dev->sdev->bus->pcicore,
					       dev->dev->sdev);
		break;
#endif
	}

	b43_imcfglo_timeouts_workaround(dev);
	b43_bluetooth_coext_disable(dev);
	if (phy->ops->prepare_hardware) {
		err = phy->ops->prepare_hardware(dev);
		if (err)
			goto err_busdown;
	}
	err = b43_chip_init(dev);
	if (err)
		goto err_busdown;
	b43_shm_write16(dev, B43_SHM_SHARED,
			B43_SHM_SH_WLCOREREV, dev->dev->core_rev);
	hf = b43_hf_read(dev);
	if (phy->type == B43_PHYTYPE_G) {
		hf |= B43_HF_SYMW;
		if (phy->rev == 1)
			hf |= B43_HF_GDCW;
		if (sprom->boardflags_lo & B43_BFL_PACTRL)
			hf |= B43_HF_OFDMPABOOST;
	}
	if (phy->radio_ver == 0x2050) {
		if (phy->radio_rev == 6)
			hf |= B43_HF_4318TSSI;
		if (phy->radio_rev < 6)
			hf |= B43_HF_VCORECALC;
	}
	if (sprom->boardflags_lo & B43_BFL_XTAL_NOSLOW)
		hf |= B43_HF_DSCRQ; /* Disable slowclock requests from ucode. */
#ifdef CONFIG_SSB_DRIVER_PCICORE
	if (dev->dev->bus_type == B43_BUS_SSB &&
	    dev->dev->sdev->bus->bustype == SSB_BUSTYPE_PCI &&
	    dev->dev->sdev->bus->pcicore.dev->id.revision <= 10)
		hf |= B43_HF_PCISCW; /* PCI slow clock workaround. */
#endif
	hf &= ~B43_HF_SKCFPUP;
	b43_hf_write(dev, hf);

	b43_set_retry_limits(dev, B43_DEFAULT_SHORT_RETRY_LIMIT,
			     B43_DEFAULT_LONG_RETRY_LIMIT);
	b43_shm_write16(dev, B43_SHM_SHARED, B43_SHM_SH_SFFBLIM, 3);
	b43_shm_write16(dev, B43_SHM_SHARED, B43_SHM_SH_LFFBLIM, 2);

	/* Disable sending probe responses from firmware.
	 * Setting the MaxTime to one usec will always trigger
	 * a timeout, so we never send any probe resp.
	 * A timeout of zero is infinite. */
	b43_shm_write16(dev, B43_SHM_SHARED, B43_SHM_SH_PRMAXTIME, 1);

	b43_rate_memory_init(dev);
	b43_set_phytxctl_defaults(dev);

	/* Minimum Contention Window */
	if (phy->type == B43_PHYTYPE_B)
		b43_shm_write16(dev, B43_SHM_SCRATCH, B43_SHM_SC_MINCONT, 0x1F);
	else
		b43_shm_write16(dev, B43_SHM_SCRATCH, B43_SHM_SC_MINCONT, 0xF);
	/* Maximum Contention Window */
	b43_shm_write16(dev, B43_SHM_SCRATCH, B43_SHM_SC_MAXCONT, 0x3FF);

	if (b43_bus_host_is_pcmcia(dev->dev) ||
	    b43_bus_host_is_sdio(dev->dev)) {
		dev->__using_pio_transfers = true;
		err = b43_pio_init(dev);
	} else if (dev->use_pio) {
		b43warn(dev->wl, "Forced PIO by use_pio module parameter. "
			"This should not be needed and will result in lower "
			"performance.\n");
		dev->__using_pio_transfers = true;
		err = b43_pio_init(dev);
	} else {
		dev->__using_pio_transfers = false;
		err = b43_dma_init(dev);
	}
	if (err)
		goto err_chip_exit;
	b43_qos_init(dev);
	b43_set_synth_pu_delay(dev, 1);
	b43_bluetooth_coext_enable(dev);

	b43_bus_powerup(dev, !(sprom->boardflags_lo & B43_BFL_XTAL_NOSLOW));
	b43_upload_card_macaddress(dev);
	b43_security_init(dev);

	ieee80211_wake_queues(dev->wl->hw);

	b43_set_status(dev, B43_STAT_INITIALIZED);

	/* Register HW RNG driver */
	b43_rng_init(dev->wl);

out:
	return err;

err_chip_exit:
	b43_chip_exit(dev);
err_busdown:
	b43_bus_may_powerdown(dev);
	B43_WARN_ON(b43_status(dev) != B43_STAT_UNINIT);
	return err;
}