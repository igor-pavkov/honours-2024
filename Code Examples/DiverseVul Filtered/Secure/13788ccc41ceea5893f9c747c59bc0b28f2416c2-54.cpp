static void run_hrtimer_softirq(struct softirq_action *h)
{
	struct hrtimer_cpu_base *cpu_base = &__get_cpu_var(hrtimer_bases);

	spin_lock_irq(&cpu_base->lock);

	while (!list_empty(&cpu_base->cb_pending)) {
		enum hrtimer_restart (*fn)(struct hrtimer *);
		struct hrtimer *timer;
		int restart;

		timer = list_entry(cpu_base->cb_pending.next,
				   struct hrtimer, cb_entry);

		timer_stats_account_hrtimer(timer);

		fn = timer->function;
		__remove_hrtimer(timer, timer->base, HRTIMER_STATE_CALLBACK, 0);
		spin_unlock_irq(&cpu_base->lock);

		restart = fn(timer);

		spin_lock_irq(&cpu_base->lock);

		timer->state &= ~HRTIMER_STATE_CALLBACK;
		if (restart == HRTIMER_RESTART) {
			BUG_ON(hrtimer_active(timer));
			/*
			 * Enqueue the timer, allow reprogramming of the event
			 * device
			 */
			enqueue_hrtimer(timer, timer->base, 1);
		} else if (hrtimer_active(timer)) {
			/*
			 * If the timer was rearmed on another CPU, reprogram
			 * the event device.
			 */
			if (timer->base->first == &timer->node)
				hrtimer_reprogram(timer, timer->base);
		}
	}
	spin_unlock_irq(&cpu_base->lock);
}