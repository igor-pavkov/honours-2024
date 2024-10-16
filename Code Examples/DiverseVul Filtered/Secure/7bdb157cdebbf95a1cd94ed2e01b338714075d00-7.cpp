static void perf_put_aux_event(struct perf_event *event)
{
	struct perf_event_context *ctx = event->ctx;
	struct perf_cpu_context *cpuctx = __get_cpu_context(ctx);
	struct perf_event *iter;

	/*
	 * If event uses aux_event tear down the link
	 */
	if (event->aux_event) {
		iter = event->aux_event;
		event->aux_event = NULL;
		put_event(iter);
		return;
	}

	/*
	 * If the event is an aux_event, tear down all links to
	 * it from other events.
	 */
	for_each_sibling_event(iter, event->group_leader) {
		if (iter->aux_event != event)
			continue;

		iter->aux_event = NULL;
		put_event(event);

		/*
		 * If it's ACTIVE, schedule it out and put it into ERROR
		 * state so that we don't try to schedule it again. Note
		 * that perf_event_enable() will clear the ERROR status.
		 */
		event_sched_out(iter, cpuctx, ctx);
		perf_event_set_state(event, PERF_EVENT_STATE_ERROR);
	}
}