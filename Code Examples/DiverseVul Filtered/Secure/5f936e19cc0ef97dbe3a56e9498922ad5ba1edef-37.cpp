static s64 alarm_timer_forward(struct k_itimer *timr, ktime_t now)
{
	struct alarm *alarm = &timr->it.alarm.alarmtimer;

	return alarm_forward(alarm, timr->it_interval, now);
}