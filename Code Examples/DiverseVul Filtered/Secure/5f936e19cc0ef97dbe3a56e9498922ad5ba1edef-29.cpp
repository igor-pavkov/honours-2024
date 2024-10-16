static int alarm_timer_create(struct k_itimer *new_timer)
{
	enum  alarmtimer_type type;

	if (!alarmtimer_get_rtcdev())
		return -ENOTSUPP;

	if (!capable(CAP_WAKE_ALARM))
		return -EPERM;

	type = clock2alarm(new_timer->it_clock);
	alarm_init(&new_timer->it.alarm.alarmtimer, type, alarm_handle_timer);
	return 0;
}