static int __maybe_unused smtcfb_pci_resume(struct device *device)
{
	struct smtcfb_info *sfb = dev_get_drvdata(device);


	/* reinit hardware */
	sm7xx_init_hw();
	switch (sfb->chip_id) {
	case 0x710:
	case 0x712:
		/* set MCLK = 14.31818 *  (0x16 / 0x2) */
		smtc_seqw(0x6a, 0x16);
		smtc_seqw(0x6b, 0x02);
		smtc_seqw(0x62, 0x3e);
		/* enable PCI burst */
		smtc_seqw(0x17, 0x20);
		if (sfb->fb->var.bits_per_pixel == 32)
			seqw17();
		break;
	case 0x720:
		smtc_seqw(0x62, 0xff);
		smtc_seqw(0x6a, 0x0d);
		smtc_seqw(0x6b, 0x02);
		break;
	}

	smtc_seqw(0x34, (smtc_seqr(0x34) | 0xc0));
	smtc_seqw(0x33, ((smtc_seqr(0x33) | 0x08) & 0xfb));

	smtcfb_setmode(sfb);

	console_lock();
	fb_set_suspend(sfb->fb, 0);
	console_unlock();

	return 0;
}