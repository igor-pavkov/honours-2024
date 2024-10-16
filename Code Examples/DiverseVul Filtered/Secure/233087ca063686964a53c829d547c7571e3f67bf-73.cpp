static void check_wp(int fdc, int drive)
{
	if (test_bit(FD_VERIFY_BIT, &drive_state[drive].flags)) {
					/* check write protection */
		output_byte(fdc, FD_GETSTATUS);
		output_byte(fdc, UNIT(drive));
		if (result(fdc) != 1) {
			fdc_state[fdc].reset = 1;
			return;
		}
		clear_bit(FD_VERIFY_BIT, &drive_state[drive].flags);
		clear_bit(FD_NEED_TWADDLE_BIT,
			  &drive_state[drive].flags);
		debug_dcl(drive_params[drive].flags,
			  "checking whether disk is write protected\n");
		debug_dcl(drive_params[drive].flags, "wp=%x\n",
			  reply_buffer[ST3] & 0x40);
		if (!(reply_buffer[ST3] & 0x40))
			set_bit(FD_DISK_WRITABLE_BIT,
				&drive_state[drive].flags);
		else
			clear_bit(FD_DISK_WRITABLE_BIT,
				  &drive_state[drive].flags);
	}
}