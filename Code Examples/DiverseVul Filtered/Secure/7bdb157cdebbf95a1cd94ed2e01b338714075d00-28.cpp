void perf_sched_cb_inc(struct pmu *pmu)
{
	struct perf_cpu_context *cpuctx = this_cpu_ptr(pmu->pmu_cpu_context);

	cpuctx->sched_cb_usage++;
}