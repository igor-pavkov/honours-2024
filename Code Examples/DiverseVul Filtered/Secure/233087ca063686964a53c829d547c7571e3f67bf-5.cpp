static void seek_interrupt(void)
{
	debugt(__func__, "");
	if (inr != 2 || (reply_buffer[ST0] & 0xF8) != 0x20) {
		DPRINT("seek failed\n");
		drive_state[current_drive].track = NEED_2_RECAL;
		cont->error();
		cont->redo();
		return;
	}
	if (drive_state[current_drive].track >= 0 &&
	    drive_state[current_drive].track != reply_buffer[ST1] &&
	    !blind_seek) {
		debug_dcl(drive_params[current_drive].flags,
			  "clearing NEWCHANGE flag because of effective seek\n");
		debug_dcl(drive_params[current_drive].flags, "jiffies=%lu\n",
			  jiffies);
		clear_bit(FD_DISK_NEWCHANGE_BIT,
			  &drive_state[current_drive].flags);
					/* effective seek */
		drive_state[current_drive].select_date = jiffies;
	}
	drive_state[current_drive].track = reply_buffer[ST1];
	floppy_ready();
}