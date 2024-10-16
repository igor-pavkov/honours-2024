static int smtc_map_smem(struct smtcfb_info *sfb,
			 struct pci_dev *pdev, u_long smem_len)
{
	sfb->fb->fix.smem_start = pci_resource_start(pdev, 0);

	if (sfb->chip_id == 0x720)
		/* on SM720, the framebuffer starts at the 1 MB offset */
		sfb->fb->fix.smem_start += 0x00200000;

	/* XXX: is it safe for SM720 on Big-Endian? */
	if (sfb->fb->var.bits_per_pixel == 32)
		sfb->fb->fix.smem_start += big_addr;

	sfb->fb->fix.smem_len = smem_len;

	sfb->fb->screen_base = sfb->lfb;

	if (!sfb->fb->screen_base) {
		dev_err(&pdev->dev,
			"%s: unable to map screen memory\n", sfb->fb->fix.id);
		return -ENOMEM;
	}

	return 0;
}