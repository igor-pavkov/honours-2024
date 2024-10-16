static ktime_t alarm_timer_remaining(struct k_itimer *timr, ktime_t now)
{
	struct alarm *alarm = &timr->it.alarm.alarmtimer;

	return ktime_sub(now, alarm->node.expires);
}