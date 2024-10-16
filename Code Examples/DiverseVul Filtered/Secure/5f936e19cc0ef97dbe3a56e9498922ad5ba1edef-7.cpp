static int alarm_clock_getres(const clockid_t which_clock, struct timespec64 *tp)
{
	if (!alarmtimer_get_rtcdev())
		return -EINVAL;

	tp->tv_sec = 0;
	tp->tv_nsec = hrtimer_resolution;
	return 0;
}