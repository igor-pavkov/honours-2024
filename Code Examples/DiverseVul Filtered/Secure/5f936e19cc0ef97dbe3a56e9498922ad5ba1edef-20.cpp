static int alarmtimer_do_nsleep(struct alarm *alarm, ktime_t absexp,
				enum alarmtimer_type type)
{
	struct restart_block *restart;
	alarm->data = (void *)current;
	do {
		set_current_state(TASK_INTERRUPTIBLE);
		alarm_start(alarm, absexp);
		if (likely(alarm->data))
			schedule();

		alarm_cancel(alarm);
	} while (alarm->data && !signal_pending(current));

	__set_current_state(TASK_RUNNING);

	destroy_hrtimer_on_stack(&alarm->timer);

	if (!alarm->data)
		return 0;

	if (freezing(current))
		alarmtimer_freezerset(absexp, type);
	restart = &current->restart_block;
	if (restart->nanosleep.type != TT_NONE) {
		struct timespec64 rmt;
		ktime_t rem;

		rem = ktime_sub(absexp, alarm_bases[type].gettime());

		if (rem <= 0)
			return 0;
		rmt = ktime_to_timespec64(rem);

		return nanosleep_copyout(restart, &rmt);
	}
	return -ERESTART_RESTARTBLOCK;
}