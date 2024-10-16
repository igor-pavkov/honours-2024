static int next_valid_format(int drive)
{
	int probed_format;

	probed_format = drive_state[drive].probed_format;
	while (1) {
		if (probed_format >= FD_AUTODETECT_SIZE ||
		    !drive_params[drive].autodetect[probed_format]) {
			drive_state[drive].probed_format = 0;
			return 1;
		}
		if (floppy_type[drive_params[drive].autodetect[probed_format]].sect) {
			drive_state[drive].probed_format = probed_format;
			return 0;
		}
		probed_format++;
	}
}