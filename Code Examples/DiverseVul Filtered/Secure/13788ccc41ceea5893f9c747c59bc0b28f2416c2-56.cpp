void unlock_hrtimer_base(const struct hrtimer *timer, unsigned long *flags)
{
	spin_unlock_irqrestore(&timer->base->cpu_base->lock, *flags);
}