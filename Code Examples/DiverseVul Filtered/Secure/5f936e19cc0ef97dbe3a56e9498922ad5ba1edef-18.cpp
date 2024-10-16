static long __sched alarm_timer_nsleep_restart(struct restart_block *restart)
{
	enum  alarmtimer_type type = restart->nanosleep.clockid;
	ktime_t exp = restart->nanosleep.expires;
	struct alarm alarm;

	alarm_init_on_stack(&alarm, type, alarmtimer_nsleep_wakeup);

	return alarmtimer_do_nsleep(&alarm, exp, type);
}