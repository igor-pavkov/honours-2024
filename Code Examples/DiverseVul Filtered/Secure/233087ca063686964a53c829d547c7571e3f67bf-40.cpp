static int floppy_resume(struct device *dev)
{
	int fdc;
	int saved_drive;

	saved_drive = current_drive;
	for (fdc = 0; fdc < N_FDC; fdc++)
		if (fdc_state[fdc].address != -1)
			user_reset_fdc(REVDRIVE(fdc, 0), FD_RESET_ALWAYS, false);
	set_fdc(saved_drive);
	return 0;
}