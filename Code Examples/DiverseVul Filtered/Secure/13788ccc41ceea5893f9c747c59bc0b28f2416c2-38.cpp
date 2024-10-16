ktime_t hrtimer_get_remaining(const struct hrtimer *timer)
{
	struct hrtimer_clock_base *base;
	unsigned long flags;
	ktime_t rem;

	base = lock_hrtimer_base(timer, &flags);
	rem = ktime_sub(timer->expires, base->get_time());
	unlock_hrtimer_base(timer, &flags);

	return rem;
}