static void ctx_resched(struct perf_cpu_context *cpuctx,
			struct perf_event_context *task_ctx,
			enum event_type_t event_type)
{
	enum event_type_t ctx_event_type;
	bool cpu_event = !!(event_type & EVENT_CPU);

	/*
	 * If pinned groups are involved, flexible groups also need to be
	 * scheduled out.
	 */
	if (event_type & EVENT_PINNED)
		event_type |= EVENT_FLEXIBLE;

	ctx_event_type = event_type & EVENT_ALL;

	perf_pmu_disable(cpuctx->ctx.pmu);
	if (task_ctx)
		task_ctx_sched_out(cpuctx, task_ctx, event_type);

	/*
	 * Decide which cpu ctx groups to schedule out based on the types
	 * of events that caused rescheduling:
	 *  - EVENT_CPU: schedule out corresponding groups;
	 *  - EVENT_PINNED task events: schedule out EVENT_FLEXIBLE groups;
	 *  - otherwise, do nothing more.
	 */
	if (cpu_event)
		cpu_ctx_sched_out(cpuctx, ctx_event_type);
	else if (ctx_event_type & EVENT_PINNED)
		cpu_ctx_sched_out(cpuctx, EVENT_FLEXIBLE);

	perf_event_sched_in(cpuctx, task_ctx, current);
	perf_pmu_enable(cpuctx->ctx.pmu);
}