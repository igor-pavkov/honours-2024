static int user_reset_fdc(int drive, int arg, bool interruptible)
{
	int ret;

	if (lock_fdc(drive))
		return -EINTR;

	if (arg == FD_RESET_ALWAYS)
		fdc_state[current_fdc].reset = 1;
	if (fdc_state[current_fdc].reset) {
		/* note: reset_fdc will take care of unlocking the driver
		 * on completion.
		 */
		cont = &reset_cont;
		ret = wait_til_done(reset_fdc, interruptible);
		if (ret == -EINTR)
			return -EINTR;
	}
	process_fd_request();
	return 0;
}