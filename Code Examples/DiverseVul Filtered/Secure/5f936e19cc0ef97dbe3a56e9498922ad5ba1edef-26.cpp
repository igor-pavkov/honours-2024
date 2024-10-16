static int alarm_timer_try_to_cancel(struct k_itimer *timr)
{
	return alarm_try_to_cancel(&timr->it.alarm.alarmtimer);
}