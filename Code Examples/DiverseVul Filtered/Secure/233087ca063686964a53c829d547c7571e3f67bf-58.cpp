static void scandrives(void)
{
	int i;
	int drive;
	int saved_drive;

	if (drive_params[current_drive].select_delay)
		return;

	saved_drive = current_drive;
	for (i = 0; i < N_DRIVE; i++) {
		drive = (saved_drive + i + 1) % N_DRIVE;
		if (drive_state[drive].fd_ref == 0 || drive_params[drive].select_delay != 0)
			continue;	/* skip closed drives */
		set_fdc(drive);
		if (!(set_dor(current_fdc, ~3, UNIT(drive) | (0x10 << UNIT(drive))) &
		      (0x10 << UNIT(drive))))
			/* switch the motor off again, if it was off to
			 * begin with */
			set_dor(current_fdc, ~(0x10 << UNIT(drive)), 0);
	}
	set_fdc(saved_drive);
}