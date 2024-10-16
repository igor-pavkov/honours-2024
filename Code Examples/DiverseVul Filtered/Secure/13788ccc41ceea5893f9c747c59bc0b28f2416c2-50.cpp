static void migrate_hrtimer_list(struct hrtimer_clock_base *old_base,
				struct hrtimer_clock_base *new_base)
{
	struct hrtimer *timer;
	struct rb_node *node;

	while ((node = rb_first(&old_base->active))) {
		timer = rb_entry(node, struct hrtimer, node);
		BUG_ON(hrtimer_callback_running(timer));
		__remove_hrtimer(timer, old_base, HRTIMER_STATE_INACTIVE, 0);
		timer->base = new_base;
		/*
		 * Enqueue the timer. Allow reprogramming of the event device
		 */
		enqueue_hrtimer(timer, new_base, 1);
	}
}