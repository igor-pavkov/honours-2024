void alarm_restart(struct alarm *alarm)
{
	struct alarm_base *base = &alarm_bases[alarm->type];
	unsigned long flags;

	spin_lock_irqsave(&base->lock, flags);
	hrtimer_set_expires(&alarm->timer, alarm->node.expires);
	hrtimer_restart(&alarm->timer);
	alarmtimer_enqueue(base, alarm);
	spin_unlock_irqrestore(&base->lock, flags);
}