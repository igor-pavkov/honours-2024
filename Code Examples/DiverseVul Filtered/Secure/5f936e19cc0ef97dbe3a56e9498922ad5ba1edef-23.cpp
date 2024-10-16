int alarm_try_to_cancel(struct alarm *alarm)
{
	struct alarm_base *base = &alarm_bases[alarm->type];
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&base->lock, flags);
	ret = hrtimer_try_to_cancel(&alarm->timer);
	if (ret >= 0)
		alarmtimer_dequeue(base, alarm);
	spin_unlock_irqrestore(&base->lock, flags);

	trace_alarmtimer_cancel(alarm, base->gettime());
	return ret;
}