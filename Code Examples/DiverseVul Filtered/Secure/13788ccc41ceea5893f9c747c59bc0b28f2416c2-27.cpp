static inline int hrtimer_enqueue_reprogram(struct hrtimer *timer,
					    struct hrtimer_clock_base *base)
{
	if (base->cpu_base->hres_active && hrtimer_reprogram(timer, base)) {

		/* Timer is expired, act upon the callback mode */
		switch(timer->cb_mode) {
		case HRTIMER_CB_IRQSAFE_NO_RESTART:
			/*
			 * We can call the callback from here. No restart
			 * happens, so no danger of recursion
			 */
			BUG_ON(timer->function(timer) != HRTIMER_NORESTART);
			return 1;
		case HRTIMER_CB_IRQSAFE_NO_SOFTIRQ:
			/*
			 * This is solely for the sched tick emulation with
			 * dynamic tick support to ensure that we do not
			 * restart the tick right on the edge and end up with
			 * the tick timer in the softirq ! The calling site
			 * takes care of this.
			 */
			return 1;
		case HRTIMER_CB_IRQSAFE:
		case HRTIMER_CB_SOFTIRQ:
			/*
			 * Move everything else into the softirq pending list !
			 */
			list_add_tail(&timer->cb_entry,
				      &base->cpu_base->cb_pending);
			timer->state = HRTIMER_STATE_PENDING;
			raise_softirq(HRTIMER_SOFTIRQ);
			return 1;
		default:
			BUG();
		}
	}
	return 0;
}