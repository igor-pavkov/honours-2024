void alarm_start_relative(struct alarm *alarm, ktime_t start)
{
	struct alarm_base *base = &alarm_bases[alarm->type];

	start = ktime_add_safe(start, base->gettime());
	alarm_start(alarm, start);
}