static void set_fdc(int drive)
{
	unsigned int fdc;

	if (drive < 0 || drive >= N_DRIVE) {
		pr_info("bad drive value %d\n", drive);
		return;
	}

	fdc = FDC(drive);
	if (fdc >= N_FDC) {
		pr_info("bad fdc value\n");
		return;
	}

	set_dor(fdc, ~0, 8);
#if N_FDC > 1
	set_dor(1 - fdc, ~8, 0);
#endif
	if (fdc_state[fdc].rawcmd == 2)
		reset_fdc_info(fdc, 1);
	if (fdc_inb(fdc, FD_STATUS) != STATUS_READY)
		fdc_state[fdc].reset = 1;

	current_drive = drive;
	current_fdc = fdc;
}