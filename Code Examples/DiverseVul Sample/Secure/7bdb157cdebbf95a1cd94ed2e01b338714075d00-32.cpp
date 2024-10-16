void perf_pmu_resched(struct pmu *pmu)
{
	struct perf_cpu_context *cpuctx = this_cpu_ptr(pmu->pmu_cpu_context);
	struct perf_event_context *task_ctx = cpuctx->task_ctx;

	perf_ctx_lock(cpuctx, task_ctx);
	ctx_resched(cpuctx, task_ctx, EVENT_ALL|EVENT_CPU);
	perf_ctx_unlock(cpuctx, task_ctx);
}