static void ring_buffer_attach(struct perf_event *event,
			       struct perf_buffer *rb)
{
	struct perf_buffer *old_rb = NULL;
	unsigned long flags;

	if (event->rb) {
		/*
		 * Should be impossible, we set this when removing
		 * event->rb_entry and wait/clear when adding event->rb_entry.
		 */
		WARN_ON_ONCE(event->rcu_pending);

		old_rb = event->rb;
		spin_lock_irqsave(&old_rb->event_lock, flags);
		list_del_rcu(&event->rb_entry);
		spin_unlock_irqrestore(&old_rb->event_lock, flags);

		event->rcu_batches = get_state_synchronize_rcu();
		event->rcu_pending = 1;
	}

	if (rb) {
		if (event->rcu_pending) {
			cond_synchronize_rcu(event->rcu_batches);
			event->rcu_pending = 0;
		}

		spin_lock_irqsave(&rb->event_lock, flags);
		list_add_rcu(&event->rb_entry, &rb->event_list);
		spin_unlock_irqrestore(&rb->event_lock, flags);
	}

	/*
	 * Avoid racing with perf_mmap_close(AUX): stop the event
	 * before swizzling the event::rb pointer; if it's getting
	 * unmapped, its aux_mmap_count will be 0 and it won't
	 * restart. See the comment in __perf_pmu_output_stop().
	 *
	 * Data will inevitably be lost when set_output is done in
	 * mid-air, but then again, whoever does it like this is
	 * not in for the data anyway.
	 */
	if (has_aux(event))
		perf_event_stop(event, 0);

	rcu_assign_pointer(event->rb, rb);

	if (old_rb) {
		ring_buffer_put(old_rb);
		/*
		 * Since we detached before setting the new rb, so that we
		 * could attach the new rb, we could have missed a wakeup.
		 * Provide it now.
		 */
		wake_up_all(&event->waitq);
	}
}