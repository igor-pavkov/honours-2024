static void floppy_start(void)
{
	reschedule_timeout(current_drive, "floppy start");

	scandrives();
	debug_dcl(drive_params[current_drive].flags,
		  "setting NEWCHANGE in floppy_start\n");
	set_bit(FD_DISK_NEWCHANGE_BIT, &drive_state[current_drive].flags);
	floppy_ready();
}