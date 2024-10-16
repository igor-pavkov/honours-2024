void hrtimer_interrupt(struct clock_event_device *dev)
{
	struct hrtimer_cpu_base *cpu_base = &__get_cpu_var(hrtimer_bases);
	struct hrtimer_clock_base *base;
	ktime_t expires_next, now;
	int i, raise = 0;

	BUG_ON(!cpu_base->hres_active);
	cpu_base->nr_events++;
	dev->next_event.tv64 = KTIME_MAX;

 retry:
	now = ktime_get();

	expires_next.tv64 = KTIME_MAX;

	base = cpu_base->clock_base;

	for (i = 0; i < HRTIMER_MAX_CLOCK_BASES; i++) {
		ktime_t basenow;
		struct rb_node *node;

		spin_lock(&cpu_base->lock);

		basenow = ktime_add(now, base->offset);

		while ((node = base->first)) {
			struct hrtimer *timer;

			timer = rb_entry(node, struct hrtimer, node);

			if (basenow.tv64 < timer->expires.tv64) {
				ktime_t expires;

				expires = ktime_sub(timer->expires,
						    base->offset);
				if (expires.tv64 < expires_next.tv64)
					expires_next = expires;
				break;
			}

			/* Move softirq callbacks to the pending list */
			if (timer->cb_mode == HRTIMER_CB_SOFTIRQ) {
				__remove_hrtimer(timer, base,
						 HRTIMER_STATE_PENDING, 0);
				list_add_tail(&timer->cb_entry,
					      &base->cpu_base->cb_pending);
				raise = 1;
				continue;
			}

			__remove_hrtimer(timer, base,
					 HRTIMER_STATE_CALLBACK, 0);
			timer_stats_account_hrtimer(timer);

			/*
			 * Note: We clear the CALLBACK bit after
			 * enqueue_hrtimer to avoid reprogramming of
			 * the event hardware. This happens at the end
			 * of this function anyway.
			 */
			if (timer->function(timer) != HRTIMER_NORESTART) {
				BUG_ON(timer->state != HRTIMER_STATE_CALLBACK);
				enqueue_hrtimer(timer, base, 0);
			}
			timer->state &= ~HRTIMER_STATE_CALLBACK;
		}
		spin_unlock(&cpu_base->lock);
		base++;
	}

	cpu_base->expires_next = expires_next;

	/* Reprogramming necessary ? */
	if (expires_next.tv64 != KTIME_MAX) {
		if (tick_program_event(expires_next, 0))
			goto retry;
	}

	/* Raise softirq ? */
	if (raise)
		raise_softirq(HRTIMER_SOFTIRQ);
}