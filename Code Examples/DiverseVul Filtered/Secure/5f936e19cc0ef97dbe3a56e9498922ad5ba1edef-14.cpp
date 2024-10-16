static enum alarmtimer_restart alarm_handle_timer(struct alarm *alarm,
							ktime_t now)
{
	struct k_itimer *ptr = container_of(alarm, struct k_itimer,
					    it.alarm.alarmtimer);
	enum alarmtimer_restart result = ALARMTIMER_NORESTART;
	unsigned long flags;
	int si_private = 0;

	spin_lock_irqsave(&ptr->it_lock, flags);

	ptr->it_active = 0;
	if (ptr->it_interval)
		si_private = ++ptr->it_requeue_pending;

	if (posix_timer_event(ptr, si_private) && ptr->it_interval) {
		/*
		 * Handle ignored signals and rearm the timer. This will go
		 * away once we handle ignored signals proper.
		 */
		ptr->it_overrun += alarm_forward_now(alarm, ptr->it_interval);
		++ptr->it_requeue_pending;
		ptr->it_active = 1;
		result = ALARMTIMER_RESTART;
	}
	spin_unlock_irqrestore(&ptr->it_lock, flags);

	return result;
}