static void _regulator_enable_delay(unsigned int delay)
{
	unsigned int ms = delay / 1000;
	unsigned int us = delay % 1000;

	if (ms > 0) {
		/*
		 * For small enough values, handle super-millisecond
		 * delays in the usleep_range() call below.
		 */
		if (ms < 20)
			us += ms * 1000;
		else
			msleep(ms);
	}

	/*
	 * Give the scheduler some room to coalesce with any other
	 * wakeup sources. For delays shorter than 10 us, don't even
	 * bother setting up high-resolution timers and just busy-
	 * loop.
	 */
	if (us >= 10)
		usleep_range(us, us + 100);
	else
		udelay(us);
}