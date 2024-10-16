static void recal_interrupt(void)
{
	debugt(__func__, "");
	if (inr != 2)
		fdc_state[current_fdc].reset = 1;
	else if (reply_buffer[ST0] & ST0_ECE) {
		switch (drive_state[current_drive].track) {
		case NEED_1_RECAL:
			debugt(__func__, "need 1 recal");
			/* after a second recalibrate, we still haven't
			 * reached track 0. Probably no drive. Raise an
			 * error, as failing immediately might upset
			 * computers possessed by the Devil :-) */
			cont->error();
			cont->redo();
			return;
		case NEED_2_RECAL:
			debugt(__func__, "need 2 recal");
			/* If we already did a recalibrate,
			 * and we are not at track 0, this
			 * means we have moved. (The only way
			 * not to move at recalibration is to
			 * be already at track 0.) Clear the
			 * new change flag */
			debug_dcl(drive_params[current_drive].flags,
				  "clearing NEWCHANGE flag because of second recalibrate\n");

			clear_bit(FD_DISK_NEWCHANGE_BIT,
				  &drive_state[current_drive].flags);
			drive_state[current_drive].select_date = jiffies;
			fallthrough;
		default:
			debugt(__func__, "default");
			/* Recalibrate moves the head by at
			 * most 80 steps. If after one
			 * recalibrate we don't have reached
			 * track 0, this might mean that we
			 * started beyond track 80.  Try
			 * again.  */
			drive_state[current_drive].track = NEED_1_RECAL;
			break;
		}
	} else
		drive_state[current_drive].track = reply_buffer[ST1];
	floppy_ready();
}