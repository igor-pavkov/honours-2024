static void alarmtimer_freezerset(ktime_t absexp, enum alarmtimer_type type)
{
	struct alarm_base *base;
	unsigned long flags;
	ktime_t delta;

	switch(type) {
	case ALARM_REALTIME:
		base = &alarm_bases[ALARM_REALTIME];
		type = ALARM_REALTIME_FREEZER;
		break;
	case ALARM_BOOTTIME:
		base = &alarm_bases[ALARM_BOOTTIME];
		type = ALARM_BOOTTIME_FREEZER;
		break;
	default:
		WARN_ONCE(1, "Invalid alarm type: %d\n", type);
		return;
	}

	delta = ktime_sub(absexp, base->gettime());

	spin_lock_irqsave(&freezer_delta_lock, flags);
	if (!freezer_delta || (delta < freezer_delta)) {
		freezer_delta = delta;
		freezer_expires = absexp;
		freezer_alarmtype = type;
	}
	spin_unlock_irqrestore(&freezer_delta_lock, flags);
}