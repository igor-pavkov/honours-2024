static int alarmtimer_suspend(struct device *dev)
{
	ktime_t min, now, expires;
	int i, ret, type;
	struct rtc_device *rtc;
	unsigned long flags;
	struct rtc_time tm;

	spin_lock_irqsave(&freezer_delta_lock, flags);
	min = freezer_delta;
	expires = freezer_expires;
	type = freezer_alarmtype;
	freezer_delta = 0;
	spin_unlock_irqrestore(&freezer_delta_lock, flags);

	rtc = alarmtimer_get_rtcdev();
	/* If we have no rtcdev, just return */
	if (!rtc)
		return 0;

	/* Find the soonest timer to expire*/
	for (i = 0; i < ALARM_NUMTYPE; i++) {
		struct alarm_base *base = &alarm_bases[i];
		struct timerqueue_node *next;
		ktime_t delta;

		spin_lock_irqsave(&base->lock, flags);
		next = timerqueue_getnext(&base->timerqueue);
		spin_unlock_irqrestore(&base->lock, flags);
		if (!next)
			continue;
		delta = ktime_sub(next->expires, base->gettime());
		if (!min || (delta < min)) {
			expires = next->expires;
			min = delta;
			type = i;
		}
	}
	if (min == 0)
		return 0;

	if (ktime_to_ns(min) < 2 * NSEC_PER_SEC) {
		__pm_wakeup_event(ws, 2 * MSEC_PER_SEC);
		return -EBUSY;
	}

	trace_alarmtimer_suspend(expires, type);

	/* Setup an rtc timer to fire that far in the future */
	rtc_timer_cancel(rtc, &rtctimer);
	rtc_read_time(rtc, &tm);
	now = rtc_tm_to_ktime(tm);
	now = ktime_add(now, min);

	/* Set alarm, if in the past reject suspend briefly to handle */
	ret = rtc_timer_start(rtc, &rtctimer, now, 0);
	if (ret < 0)
		__pm_wakeup_event(ws, MSEC_PER_SEC);
	return ret;
}