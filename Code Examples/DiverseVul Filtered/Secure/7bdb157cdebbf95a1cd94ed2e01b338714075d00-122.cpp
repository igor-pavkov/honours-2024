static void __perf_pmu_sched_task(struct perf_cpu_context *cpuctx, bool sched_in)
{
	struct pmu *pmu;

	pmu = cpuctx->ctx.pmu; /* software PMUs will not have sched_task */

	if (WARN_ON_ONCE(!pmu->sched_task))
		return;

	perf_ctx_lock(cpuctx, cpuctx->task_ctx);
	perf_pmu_disable(pmu);

	pmu->sched_task(cpuctx->task_ctx, sched_in);

	perf_pmu_enable(pmu);
	perf_ctx_unlock(cpuctx, cpuctx->task_ctx);
}