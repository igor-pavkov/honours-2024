static int fd_wait_for_completion(unsigned long expires,
				  void (*function)(void))
{
	if (fdc_state[current_fdc].reset) {
		reset_fdc();	/* do the reset during sleep to win time
				 * if we don't need to sleep, it's a good
				 * occasion anyways */
		return 1;
	}

	if (time_before(jiffies, expires)) {
		cancel_delayed_work(&fd_timer);
		fd_timer_fn = function;
		queue_delayed_work(floppy_wq, &fd_timer, expires - jiffies);
		return 1;
	}
	return 0;
}