static int smtcfb_pci_probe(struct pci_dev *pdev,
			    const struct pci_device_id *ent)
{
	struct smtcfb_info *sfb;
	struct fb_info *info;
	u_long smem_size;
	int err;
	unsigned long mmio_base;

	dev_info(&pdev->dev, "Silicon Motion display driver.\n");

	err = pci_enable_device(pdev);	/* enable SMTC chip */
	if (err)
		return err;

	err = pci_request_region(pdev, 0, "sm7xxfb");
	if (err < 0) {
		dev_err(&pdev->dev, "cannot reserve framebuffer region\n");
		goto failed_regions;
	}

	sprintf(smtcfb_fix.id, "sm%Xfb", ent->device);

	info = framebuffer_alloc(sizeof(*sfb), &pdev->dev);
	if (!info) {
		err = -ENOMEM;
		goto failed_free;
	}

	sfb = info->par;
	sfb->fb = info;
	sfb->chip_id = ent->device;
	sfb->pdev = pdev;
	info->flags = FBINFO_FLAG_DEFAULT;
	info->fbops = &smtcfb_ops;
	info->fix = smtcfb_fix;
	info->var = smtcfb_var;
	info->pseudo_palette = sfb->colreg;
	info->par = sfb;

	pci_set_drvdata(pdev, sfb);

	sm7xx_init_hw();

	/* Map address and memory detection */
	mmio_base = pci_resource_start(pdev, 0);
	pci_read_config_byte(pdev, PCI_REVISION_ID, &sfb->chip_rev_id);

	smem_size = sm7xx_vram_probe(sfb);
	dev_info(&pdev->dev, "%lu MiB of VRAM detected.\n",
					smem_size / 1048576);

	switch (sfb->chip_id) {
	case 0x710:
	case 0x712:
		sfb->fb->fix.mmio_start = mmio_base + 0x00400000;
		sfb->fb->fix.mmio_len = 0x00400000;
		sfb->lfb = ioremap(mmio_base, mmio_addr);
		if (!sfb->lfb) {
			dev_err(&pdev->dev,
				"%s: unable to map memory mapped IO!\n",
				sfb->fb->fix.id);
			err = -ENOMEM;
			goto failed_fb;
		}

		sfb->mmio = (smtc_regbaseaddress =
		    sfb->lfb + 0x00700000);
		sfb->dp_regs = sfb->lfb + 0x00408000;
		sfb->vp_regs = sfb->lfb + 0x0040c000;
		if (sfb->fb->var.bits_per_pixel == 32) {
			sfb->lfb += big_addr;
			dev_info(&pdev->dev, "sfb->lfb=%p\n", sfb->lfb);
		}

		/* set MCLK = 14.31818 * (0x16 / 0x2) */
		smtc_seqw(0x6a, 0x16);
		smtc_seqw(0x6b, 0x02);
		smtc_seqw(0x62, 0x3e);
		/* enable PCI burst */
		smtc_seqw(0x17, 0x20);
		/* enable word swap */
		if (sfb->fb->var.bits_per_pixel == 32)
			seqw17();
		break;
	case 0x720:
		sfb->fb->fix.mmio_start = mmio_base;
		sfb->fb->fix.mmio_len = 0x00200000;
		sfb->dp_regs = ioremap(mmio_base, 0x00200000 + smem_size);
		if (!sfb->dp_regs) {
			dev_err(&pdev->dev,
				"%s: unable to map memory mapped IO!\n",
				sfb->fb->fix.id);
			err = -ENOMEM;
			goto failed_fb;
		}

		sfb->lfb = sfb->dp_regs + 0x00200000;
		sfb->mmio = (smtc_regbaseaddress =
		    sfb->dp_regs + 0x000c0000);
		sfb->vp_regs = sfb->dp_regs + 0x800;

		smtc_seqw(0x62, 0xff);
		smtc_seqw(0x6a, 0x0d);
		smtc_seqw(0x6b, 0x02);
		break;
	default:
		dev_err(&pdev->dev,
			"No valid Silicon Motion display chip was detected!\n");
		err = -ENODEV;
		goto failed_fb;
	}

	/* probe and decide resolution */
	sm7xx_resolution_probe(sfb);

	/* can support 32 bpp */
	if (sfb->fb->var.bits_per_pixel == 15)
		sfb->fb->var.bits_per_pixel = 16;

	sfb->fb->var.xres_virtual = sfb->fb->var.xres;
	sfb->fb->var.yres_virtual = sfb->fb->var.yres;
	err = smtc_map_smem(sfb, pdev, smem_size);
	if (err)
		goto failed;

	/*
	 * The screen would be temporarily garbled when sm712fb takes over
	 * vesafb or VGA text mode. Zero the framebuffer.
	 */
	memset_io(sfb->lfb, 0, sfb->fb->fix.smem_len);

	err = register_framebuffer(info);
	if (err < 0)
		goto failed;

	dev_info(&pdev->dev,
		 "Silicon Motion SM%X Rev%X primary display mode %dx%d-%d Init Complete.\n",
		 sfb->chip_id, sfb->chip_rev_id, sfb->fb->var.xres,
		 sfb->fb->var.yres, sfb->fb->var.bits_per_pixel);

	return 0;

failed:
	dev_err(&pdev->dev, "Silicon Motion, Inc. primary display init fail.\n");

	smtc_unmap_smem(sfb);
	smtc_unmap_mmio(sfb);
failed_fb:
	framebuffer_release(info);

failed_free:
	pci_release_region(pdev, 0);

failed_regions:
	pci_disable_device(pdev);

	return err;
}