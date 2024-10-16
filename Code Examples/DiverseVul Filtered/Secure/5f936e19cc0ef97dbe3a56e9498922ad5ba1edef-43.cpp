void alarm_init(struct alarm *alarm, enum alarmtimer_type type,
		enum alarmtimer_restart (*function)(struct alarm *, ktime_t))
{
	hrtimer_init(&alarm->timer, alarm_bases[type].base_clockid,
		     HRTIMER_MODE_ABS);
	__alarm_init(alarm, type, function);
}