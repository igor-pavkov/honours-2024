static void alarm_timer_arm(struct k_itimer *timr, ktime_t expires,
			    bool absolute, bool sigev_none)
{
	struct alarm *alarm = &timr->it.alarm.alarmtimer;
	struct alarm_base *base = &alarm_bases[alarm->type];

	if (!absolute)
		expires = ktime_add_safe(expires, base->gettime());
	if (sigev_none)
		alarm->node.expires = expires;
	else
		alarm_start(&timr->it.alarm.alarmtimer, expires);
}