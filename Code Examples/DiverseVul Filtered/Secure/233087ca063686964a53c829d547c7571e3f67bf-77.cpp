static void setup_rw_floppy(void)
{
	int i;
	int r;
	int flags;
	unsigned long ready_date;
	void (*function)(void);

	flags = raw_cmd->flags;
	if (flags & (FD_RAW_READ | FD_RAW_WRITE))
		flags |= FD_RAW_INTR;

	if ((flags & FD_RAW_SPIN) && !(flags & FD_RAW_NO_MOTOR)) {
		ready_date = drive_state[current_drive].spinup_date + drive_params[current_drive].spinup;
		/* If spinup will take a long time, rerun scandrives
		 * again just before spinup completion. Beware that
		 * after scandrives, we must again wait for selection.
		 */
		if (time_after(ready_date, jiffies + drive_params[current_drive].select_delay)) {
			ready_date -= drive_params[current_drive].select_delay;
			function = floppy_start;
		} else
			function = setup_rw_floppy;

		/* wait until the floppy is spinning fast enough */
		if (fd_wait_for_completion(ready_date, function))
			return;
	}
	if ((flags & FD_RAW_READ) || (flags & FD_RAW_WRITE))
		setup_DMA();

	if (flags & FD_RAW_INTR)
		do_floppy = main_command_interrupt;

	r = 0;
	for (i = 0; i < raw_cmd->cmd_count; i++)
		r |= output_byte(current_fdc, raw_cmd->fullcmd[i]);

	debugt(__func__, "rw_command");

	if (r) {
		cont->error();
		reset_fdc();
		return;
	}

	if (!(flags & FD_RAW_INTR)) {
		inr = result(current_fdc);
		cont->interrupt();
	} else if (flags & FD_RAW_NEED_DISK)
		fd_watchdog();
}