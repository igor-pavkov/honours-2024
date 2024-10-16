static void perf_pmu_sched_task(struct task_struct *prev,
				struct task_struct *next,
				bool sched_in)
{
	struct perf_cpu_context *cpuctx;
	struct pmu *pmu;

	if (prev == next)
		return;

	list_for_each_entry(cpuctx, this_cpu_ptr(&sched_cb_list), sched_cb_entry) {
		pmu = cpuctx->ctx.pmu; /* software PMUs will not have sched_task */

		if (WARN_ON_ONCE(!pmu->sched_task))
			continue;

		perf_ctx_lock(cpuctx, cpuctx->task_ctx);
		perf_pmu_disable(pmu);

		pmu->sched_task(cpuctx->task_ctx, sched_in);

		perf_pmu_enable(pmu);
		perf_ctx_unlock(cpuctx, cpuctx->task_ctx);
	}
}