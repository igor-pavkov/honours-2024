static void alarm_timer_rearm(struct k_itimer *timr)
{
	struct alarm *alarm = &timr->it.alarm.alarmtimer;

	timr->it_overrun += alarm_forward_now(alarm, timr->it_interval);
	alarm_start(alarm, alarm->node.expires);
}