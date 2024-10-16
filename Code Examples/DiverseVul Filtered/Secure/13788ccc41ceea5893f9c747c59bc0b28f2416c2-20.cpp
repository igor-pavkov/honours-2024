static inline void run_hrtimer_queue(struct hrtimer_cpu_base *cpu_base,
				     int index)
{
	struct rb_node *node;
	struct hrtimer_clock_base *base = &cpu_base->clock_base[index];

	if (!base->first)
		return;

	if (base->get_softirq_time)
		base->softirq_time = base->get_softirq_time();

	spin_lock_irq(&cpu_base->lock);

	while ((node = base->first)) {
		struct hrtimer *timer;
		enum hrtimer_restart (*fn)(struct hrtimer *);
		int restart;

		timer = rb_entry(node, struct hrtimer, node);
		if (base->softirq_time.tv64 <= timer->expires.tv64)
			break;

#ifdef CONFIG_HIGH_RES_TIMERS
		WARN_ON_ONCE(timer->cb_mode == HRTIMER_CB_IRQSAFE_NO_SOFTIRQ);
#endif
		timer_stats_account_hrtimer(timer);

		fn = timer->function;
		__remove_hrtimer(timer, base, HRTIMER_STATE_CALLBACK, 0);
		spin_unlock_irq(&cpu_base->lock);

		restart = fn(timer);

		spin_lock_irq(&cpu_base->lock);

		timer->state &= ~HRTIMER_STATE_CALLBACK;
		if (restart != HRTIMER_NORESTART) {
			BUG_ON(hrtimer_active(timer));
			enqueue_hrtimer(timer, base, 0);
		}
	}
	spin_unlock_irq(&cpu_base->lock);
}