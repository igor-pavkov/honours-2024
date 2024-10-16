static int wait_til_done(void (*handler)(void), bool interruptible)
{
	int ret;

	schedule_bh(handler);

	if (interruptible)
		wait_event_interruptible(command_done, command_status >= 2);
	else
		wait_event(command_done, command_status >= 2);

	if (command_status < 2) {
		cancel_activity();
		cont = &intr_cont;
		reset_fdc();
		return -EINTR;
	}

	if (fdc_state[current_fdc].reset)
		command_status = FD_COMMAND_ERROR;
	if (command_status == FD_COMMAND_OKAY)
		ret = 0;
	else
		ret = -EIO;
	command_status = FD_COMMAND_NONE;
	return ret;
}