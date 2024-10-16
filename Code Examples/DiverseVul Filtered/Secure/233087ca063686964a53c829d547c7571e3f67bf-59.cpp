static void floppy_ready(void)
{
	if (fdc_state[current_fdc].reset) {
		reset_fdc();
		return;
	}
	if (start_motor(floppy_ready))
		return;
	if (fdc_dtr())
		return;

	debug_dcl(drive_params[current_drive].flags,
		  "calling disk change from floppy_ready\n");
	if (!(raw_cmd->flags & FD_RAW_NO_MOTOR) &&
	    disk_change(current_drive) && !drive_params[current_drive].select_delay)
		twaddle(current_fdc, current_drive);	/* this clears the dcl on certain
				 * drive/controller combinations */

#ifdef fd_chose_dma_mode
	if ((raw_cmd->flags & FD_RAW_READ) || (raw_cmd->flags & FD_RAW_WRITE)) {
		unsigned long flags = claim_dma_lock();
		fd_chose_dma_mode(raw_cmd->kernel_data, raw_cmd->length);
		release_dma_lock(flags);
	}
#endif

	if (raw_cmd->flags & (FD_RAW_NEED_SEEK | FD_RAW_NEED_DISK)) {
		perpendicular_mode(current_fdc);
		fdc_specify(current_fdc, current_drive); /* must be done here because of hut, hlt ... */
		seek_floppy();
	} else {
		if ((raw_cmd->flags & FD_RAW_READ) ||
		    (raw_cmd->flags & FD_RAW_WRITE))
			fdc_specify(current_fdc, current_drive);
		setup_rw_floppy();
	}
}