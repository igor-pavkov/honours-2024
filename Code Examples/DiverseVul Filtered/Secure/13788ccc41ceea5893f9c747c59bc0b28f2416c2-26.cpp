static void __remove_hrtimer(struct hrtimer *timer,
			     struct hrtimer_clock_base *base,
			     unsigned long newstate, int reprogram)
{
	/* High res. callback list. NOP for !HIGHRES */
	if (hrtimer_cb_pending(timer))
		hrtimer_remove_cb_pending(timer);
	else {
		/*
		 * Remove the timer from the rbtree and replace the
		 * first entry pointer if necessary.
		 */
		if (base->first == &timer->node) {
			base->first = rb_next(&timer->node);
			/* Reprogram the clock event device. if enabled */
			if (reprogram && hrtimer_hres_active())
				hrtimer_force_reprogram(base->cpu_base);
		}
		rb_erase(&timer->node, &base->active);
	}
	timer->state = newstate;
}