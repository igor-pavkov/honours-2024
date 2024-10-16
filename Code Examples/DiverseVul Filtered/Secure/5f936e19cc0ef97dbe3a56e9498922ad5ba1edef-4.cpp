static int alarm_clock_get(clockid_t which_clock, struct timespec64 *tp)
{
	struct alarm_base *base = &alarm_bases[clock2alarm(which_clock)];

	if (!alarmtimer_get_rtcdev())
		return -EINVAL;

	*tp = ktime_to_timespec64(base->gettime());
	return 0;
}