static int start_motor(void (*function)(void))
{
	int mask;
	int data;

	mask = 0xfc;
	data = UNIT(current_drive);
	if (!(raw_cmd->flags & FD_RAW_NO_MOTOR)) {
		if (!(fdc_state[current_fdc].dor & (0x10 << UNIT(current_drive)))) {
			set_debugt();
			/* no read since this drive is running */
			drive_state[current_drive].first_read_date = 0;
			/* note motor start time if motor is not yet running */
			drive_state[current_drive].spinup_date = jiffies;
			data |= (0x10 << UNIT(current_drive));
		}
	} else if (fdc_state[current_fdc].dor & (0x10 << UNIT(current_drive)))
		mask &= ~(0x10 << UNIT(current_drive));

	/* starts motor and selects floppy */
	del_timer(motor_off_timer + current_drive);
	set_dor(current_fdc, mask, data);

	/* wait_for_completion also schedules reset if needed. */
	return fd_wait_for_completion(drive_state[current_drive].select_date + drive_params[current_drive].select_delay,
				      function);
}