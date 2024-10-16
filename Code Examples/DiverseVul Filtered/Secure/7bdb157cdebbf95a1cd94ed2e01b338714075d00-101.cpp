static void _perf_event_enable(struct perf_event *event)
{
	struct perf_event_context *ctx = event->ctx;

	raw_spin_lock_irq(&ctx->lock);
	if (event->state >= PERF_EVENT_STATE_INACTIVE ||
	    event->state <  PERF_EVENT_STATE_ERROR) {
out:
		raw_spin_unlock_irq(&ctx->lock);
		return;
	}

	/*
	 * If the event is in error state, clear that first.
	 *
	 * That way, if we see the event in error state below, we know that it
	 * has gone back into error state, as distinct from the task having
	 * been scheduled away before the cross-call arrived.
	 */
	if (event->state == PERF_EVENT_STATE_ERROR) {
		/*
		 * Detached SIBLING events cannot leave ERROR state.
		 */
		if (event->event_caps & PERF_EV_CAP_SIBLING &&
		    event->group_leader == event)
			goto out;

		event->state = PERF_EVENT_STATE_OFF;
	}
	raw_spin_unlock_irq(&ctx->lock);

	event_function_call(event, __perf_event_enable, NULL);
}