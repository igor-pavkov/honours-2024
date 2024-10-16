static void floppy_off(unsigned int drive)
{
	unsigned long volatile delta;
	int fdc = FDC(drive);

	if (!(fdc_state[fdc].dor & (0x10 << UNIT(drive))))
		return;

	del_timer(motor_off_timer + drive);

	/* make spindle stop in a position which minimizes spinup time
	 * next time */
	if (drive_params[drive].rps) {
		delta = jiffies - drive_state[drive].first_read_date + HZ -
		    drive_params[drive].spindown_offset;
		delta = ((delta * drive_params[drive].rps) % HZ) / drive_params[drive].rps;
		motor_off_timer[drive].expires =
		    jiffies + drive_params[drive].spindown - delta;
	}
	add_timer(motor_off_timer + drive);
}