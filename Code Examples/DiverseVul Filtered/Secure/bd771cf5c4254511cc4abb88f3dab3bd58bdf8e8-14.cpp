static void sm7xx_set_timing(struct smtcfb_info *sfb)
{
	int i = 0, j = 0;
	u32 m_nscreenstride;

	dev_dbg(&sfb->pdev->dev,
		"sfb->width=%d sfb->height=%d sfb->fb->var.bits_per_pixel=%d sfb->hz=%d\n",
		sfb->width, sfb->height, sfb->fb->var.bits_per_pixel, sfb->hz);

	for (j = 0; j < ARRAY_SIZE(vgamode); j++) {
		if (vgamode[j].mmsizex != sfb->width ||
		    vgamode[j].mmsizey != sfb->height ||
		    vgamode[j].bpp != sfb->fb->var.bits_per_pixel ||
		    vgamode[j].hz != sfb->hz)
			continue;

		dev_dbg(&sfb->pdev->dev,
			"vgamode[j].mmsizex=%d vgamode[j].mmSizeY=%d vgamode[j].bpp=%d vgamode[j].hz=%d\n",
			vgamode[j].mmsizex, vgamode[j].mmsizey,
			vgamode[j].bpp, vgamode[j].hz);

		dev_dbg(&sfb->pdev->dev, "vgamode index=%d\n", j);

		smtc_mmiowb(0x0, 0x3c6);

		smtc_seqw(0, 0x1);

		smtc_mmiowb(vgamode[j].init_misc, 0x3c2);

		/* init SEQ register SR00 - SR04 */
		for (i = 0; i < SIZE_SR00_SR04; i++)
			smtc_seqw(i, vgamode[j].init_sr00_sr04[i]);

		/* init SEQ register SR10 - SR24 */
		for (i = 0; i < SIZE_SR10_SR24; i++)
			smtc_seqw(i + 0x10, vgamode[j].init_sr10_sr24[i]);

		/* init SEQ register SR30 - SR75 */
		for (i = 0; i < SIZE_SR30_SR75; i++)
			if ((i + 0x30) != 0x30 && (i + 0x30) != 0x62 &&
			    (i + 0x30) != 0x6a && (i + 0x30) != 0x6b &&
			    (i + 0x30) != 0x70 && (i + 0x30) != 0x71 &&
			    (i + 0x30) != 0x74 && (i + 0x30) != 0x75)
				smtc_seqw(i + 0x30,
					  vgamode[j].init_sr30_sr75[i]);

		/* init SEQ register SR80 - SR93 */
		for (i = 0; i < SIZE_SR80_SR93; i++)
			smtc_seqw(i + 0x80, vgamode[j].init_sr80_sr93[i]);

		/* init SEQ register SRA0 - SRAF */
		for (i = 0; i < SIZE_SRA0_SRAF; i++)
			smtc_seqw(i + 0xa0, vgamode[j].init_sra0_sraf[i]);

		/* init Graphic register GR00 - GR08 */
		for (i = 0; i < SIZE_GR00_GR08; i++)
			smtc_grphw(i, vgamode[j].init_gr00_gr08[i]);

		/* init Attribute register AR00 - AR14 */
		for (i = 0; i < SIZE_AR00_AR14; i++)
			smtc_attrw(i, vgamode[j].init_ar00_ar14[i]);

		/* init CRTC register CR00 - CR18 */
		for (i = 0; i < SIZE_CR00_CR18; i++)
			smtc_crtcw(i, vgamode[j].init_cr00_cr18[i]);

		/* init CRTC register CR30 - CR4D */
		for (i = 0; i < SIZE_CR30_CR4D; i++) {
			if ((i + 0x30) >= 0x3B && (i + 0x30) <= 0x3F)
				/* side-effect, don't write to CR3B-CR3F */
				continue;
			smtc_crtcw(i + 0x30, vgamode[j].init_cr30_cr4d[i]);
		}

		/* init CRTC register CR90 - CRA7 */
		for (i = 0; i < SIZE_CR90_CRA7; i++)
			smtc_crtcw(i + 0x90, vgamode[j].init_cr90_cra7[i]);
	}
	smtc_mmiowb(0x67, 0x3c2);

	/* set VPR registers */
	writel(0x0, sfb->vp_regs + 0x0C);
	writel(0x0, sfb->vp_regs + 0x40);

	/* set data width */
	m_nscreenstride = (sfb->width * sfb->fb->var.bits_per_pixel) / 64;
	switch (sfb->fb->var.bits_per_pixel) {
	case 8:
		writel(0x0, sfb->vp_regs + 0x0);
		break;
	case 16:
		writel(0x00020000, sfb->vp_regs + 0x0);
		break;
	case 24:
		writel(0x00040000, sfb->vp_regs + 0x0);
		break;
	case 32:
		writel(0x00030000, sfb->vp_regs + 0x0);
		break;
	}
	writel((u32)(((m_nscreenstride + 2) << 16) | m_nscreenstride),
	       sfb->vp_regs + 0x10);
}