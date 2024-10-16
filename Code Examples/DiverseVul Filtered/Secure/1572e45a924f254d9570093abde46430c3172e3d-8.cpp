static int perf_event_read(struct perf_event *event, bool group)
{
	int event_cpu, ret = 0;

	/*
	 * If event is enabled and currently active on a CPU, update the
	 * value in the event structure:
	 */
	if (event->state == PERF_EVENT_STATE_ACTIVE) {
		struct perf_read_data data = {
			.event = event,
			.group = group,
			.ret = 0,
		};

		event_cpu = READ_ONCE(event->oncpu);
		if ((unsigned)event_cpu >= nr_cpu_ids)
			return 0;

		preempt_disable();
		event_cpu = __perf_event_read_cpu(event, event_cpu);

		/*
		 * Purposely ignore the smp_call_function_single() return
		 * value.
		 *
		 * If event_cpu isn't a valid CPU it means the event got
		 * scheduled out and that will have updated the event count.
		 *
		 * Therefore, either way, we'll have an up-to-date event count
		 * after this.
		 */
		(void)smp_call_function_single(event_cpu, __perf_event_read, &data, 1);
		preempt_enable();
		ret = data.ret;
	} else if (event->state == PERF_EVENT_STATE_INACTIVE) {
		struct perf_event_context *ctx = event->ctx;
		unsigned long flags;

		raw_spin_lock_irqsave(&ctx->lock, flags);
		/*
		 * may read while context is not active
		 * (e.g., thread is blocked), in that case
		 * we cannot update context time
		 */
		if (ctx->is_active) {
			update_context_time(ctx);
			update_cgrp_time_from_event(event);
		}
		if (group)
			update_group_times(event);
		else
			update_event_times(event);
		raw_spin_unlock_irqrestore(&ctx->lock, flags);
	}

	return ret;
}