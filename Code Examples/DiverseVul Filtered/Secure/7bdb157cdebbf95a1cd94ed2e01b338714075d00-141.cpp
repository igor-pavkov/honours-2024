static void perf_pending_event(struct irq_work *entry)
{
	struct perf_event *event = container_of(entry, struct perf_event, pending);
	int rctx;

	rctx = perf_swevent_get_recursion_context();
	/*
	 * If we 'fail' here, that's OK, it means recursion is already disabled
	 * and we won't recurse 'further'.
	 */

	perf_pending_event_disable(event);

	if (event->pending_wakeup) {
		event->pending_wakeup = 0;
		perf_event_wakeup(event);
	}

	if (rctx >= 0)
		perf_swevent_put_recursion_context(rctx);
}