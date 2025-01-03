int perf_event_read_local(struct perf_event *event, u64 *value,
			  u64 *enabled, u64 *running)
{
	unsigned long flags;
	int ret = 0;

	/*
	 * Disabling interrupts avoids all counter scheduling (context
	 * switches, timer based rotation and IPIs).
	 */
	local_irq_save(flags);

	/*
	 * It must not be an event with inherit set, we cannot read
	 * all child counters from atomic context.
	 */
	if (event->attr.inherit) {
		ret = -EOPNOTSUPP;
		goto out;
	}

	/* If this is a per-task event, it must be for current */
	if ((event->attach_state & PERF_ATTACH_TASK) &&
	    event->hw.target != current) {
		ret = -EINVAL;
		goto out;
	}

	/* If this is a per-CPU event, it must be for this CPU */
	if (!(event->attach_state & PERF_ATTACH_TASK) &&
	    event->cpu != smp_processor_id()) {
		ret = -EINVAL;
		goto out;
	}

	/* If this is a pinned event it must be running on this CPU */
	if (event->attr.pinned && event->oncpu != smp_processor_id()) {
		ret = -EBUSY;
		goto out;
	}

	/*
	 * If the event is currently on this CPU, its either a per-task event,
	 * or local to this CPU. Furthermore it means its ACTIVE (otherwise
	 * oncpu == -1).
	 */
	if (event->oncpu == smp_processor_id())
		event->pmu->read(event);

	*value = local64_read(&event->count);
	if (enabled || running) {
		u64 now = event->shadow_ctx_time + perf_clock();
		u64 __enabled, __running;

		__perf_update_times(event, now, &__enabled, &__running);
		if (enabled)
			*enabled = __enabled;
		if (running)
			*running = __running;
	}
out:
	local_irq_restore(flags);

	return ret;
}