static void task_ctx_sched_out(struct perf_cpu_context *cpuctx,
			       struct perf_event_context *ctx,
			       enum event_type_t event_type)
{
	if (!cpuctx->task_ctx)
		return;

	if (WARN_ON_ONCE(ctx != cpuctx->task_ctx))
		return;

	ctx_sched_out(ctx, cpuctx, event_type);
}