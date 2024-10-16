static int __maybe_unused smtcfb_pci_suspend(struct device *device)
{
	struct smtcfb_info *sfb = dev_get_drvdata(device);


	/* set the hw in sleep mode use external clock and self memory refresh
	 * so that we can turn off internal PLLs later on
	 */
	smtc_seqw(0x20, (smtc_seqr(0x20) | 0xc0));
	smtc_seqw(0x69, (smtc_seqr(0x69) & 0xf7));

	console_lock();
	fb_set_suspend(sfb->fb, 1);
	console_unlock();

	/* additionally turn off all function blocks including internal PLLs */
	smtc_seqw(0x21, 0xff);

	return 0;
}