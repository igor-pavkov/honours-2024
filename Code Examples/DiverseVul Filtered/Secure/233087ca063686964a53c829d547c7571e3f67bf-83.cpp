static void seek_floppy(void)
{
	int track;

	blind_seek = 0;

	debug_dcl(drive_params[current_drive].flags,
		  "calling disk change from %s\n", __func__);

	if (!test_bit(FD_DISK_NEWCHANGE_BIT, &drive_state[current_drive].flags) &&
	    disk_change(current_drive) && (raw_cmd->flags & FD_RAW_NEED_DISK)) {
		/* the media changed flag should be cleared after the seek.
		 * If it isn't, this means that there is really no disk in
		 * the drive.
		 */
		set_bit(FD_DISK_CHANGED_BIT,
			&drive_state[current_drive].flags);
		cont->done(0);
		cont->redo();
		return;
	}
	if (drive_state[current_drive].track <= NEED_1_RECAL) {
		recalibrate_floppy();
		return;
	} else if (test_bit(FD_DISK_NEWCHANGE_BIT, &drive_state[current_drive].flags) &&
		   (raw_cmd->flags & FD_RAW_NEED_DISK) &&
		   (drive_state[current_drive].track <= NO_TRACK || drive_state[current_drive].track == raw_cmd->track)) {
		/* we seek to clear the media-changed condition. Does anybody
		 * know a more elegant way, which works on all drives? */
		if (raw_cmd->track)
			track = raw_cmd->track - 1;
		else {
			if (drive_params[current_drive].flags & FD_SILENT_DCL_CLEAR) {
				set_dor(current_fdc, ~(0x10 << UNIT(current_drive)), 0);
				blind_seek = 1;
				raw_cmd->flags |= FD_RAW_NEED_SEEK;
			}
			track = 1;
		}
	} else {
		check_wp(current_fdc, current_drive);
		if (raw_cmd->track != drive_state[current_drive].track &&
		    (raw_cmd->flags & FD_RAW_NEED_SEEK))
			track = raw_cmd->track;
		else {
			setup_rw_floppy();
			return;
		}
	}

	do_floppy = seek_interrupt;
	output_byte(current_fdc, FD_SEEK);
	output_byte(current_fdc, UNIT(current_drive));
	if (output_byte(current_fdc, track) < 0) {
		reset_fdc();
		return;
	}
	debugt(__func__, "");
}