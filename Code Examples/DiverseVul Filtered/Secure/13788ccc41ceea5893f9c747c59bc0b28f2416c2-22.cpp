static void enqueue_hrtimer(struct hrtimer *timer,
			    struct hrtimer_clock_base *base, int reprogram)
{
	struct rb_node **link = &base->active.rb_node;
	struct rb_node *parent = NULL;
	struct hrtimer *entry;

	/*
	 * Find the right place in the rbtree:
	 */
	while (*link) {
		parent = *link;
		entry = rb_entry(parent, struct hrtimer, node);
		/*
		 * We dont care about collisions. Nodes with
		 * the same expiry time stay together.
		 */
		if (timer->expires.tv64 < entry->expires.tv64)
			link = &(*link)->rb_left;
		else
			link = &(*link)->rb_right;
	}

	/*
	 * Insert the timer to the rbtree and check whether it
	 * replaces the first pending timer
	 */
	if (!base->first || timer->expires.tv64 <
	    rb_entry(base->first, struct hrtimer, node)->expires.tv64) {
		/*
		 * Reprogram the clock event device. When the timer is already
		 * expired hrtimer_enqueue_reprogram has either called the
		 * callback or added it to the pending list and raised the
		 * softirq.
		 *
		 * This is a NOP for !HIGHRES
		 */
		if (reprogram && hrtimer_enqueue_reprogram(timer, base))
			return;

		base->first = &timer->node;
	}

	rb_link_node(&timer->node, parent, link);
	rb_insert_color(&timer->node, &base->active);
	/*
	 * HRTIMER_STATE_ENQUEUED is or'ed to the current state to preserve the
	 * state of a possibly running callback.
	 */
	timer->state |= HRTIMER_STATE_ENQUEUED;
}