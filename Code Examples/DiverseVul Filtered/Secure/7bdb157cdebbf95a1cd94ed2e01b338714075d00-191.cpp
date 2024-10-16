static int __perf_pmu_output_stop(void *info)
{
	struct perf_event *event = info;
	struct pmu *pmu = event->ctx->pmu;
	struct perf_cpu_context *cpuctx = this_cpu_ptr(pmu->pmu_cpu_context);
	struct remote_output ro = {
		.rb	= event->rb,
	};

	rcu_read_lock();
	perf_iterate_ctx(&cpuctx->ctx, __perf_event_output_stop, &ro, false);
	if (cpuctx->task_ctx)
		perf_iterate_ctx(cpuctx->task_ctx, __perf_event_output_stop,
				   &ro, false);
	rcu_read_unlock();

	return ro.err;
}