static void perf_remove_from_context(struct perf_event *event, unsigned long flags)
{
	struct perf_event_context *ctx = event->ctx;

	lockdep_assert_held(&ctx->mutex);

	event_function_call(event, __perf_remove_from_context, (void *)flags);

	/*
	 * The above event_function_call() can NO-OP when it hits
	 * TASK_TOMBSTONE. In that case we must already have been detached
	 * from the context (by perf_event_exit_event()) but the grouping
	 * might still be in-tact.
	 */
	WARN_ON_ONCE(event->attach_state & PERF_ATTACH_CONTEXT);
	if ((flags & DETACH_GROUP) &&
	    (event->attach_state & PERF_ATTACH_GROUP)) {
		/*
		 * Since in that case we cannot possibly be scheduled, simply
		 * detach now.
		 */
		raw_spin_lock_irq(&ctx->lock);
		perf_group_detach(event);
		raw_spin_unlock_irq(&ctx->lock);
	}
}