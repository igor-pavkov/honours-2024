int perf_event_init_cpu(unsigned int cpu)
{
	struct perf_cpu_context *cpuctx;
	struct perf_event_context *ctx;
	struct pmu *pmu;

	perf_swevent_init_cpu(cpu);

	mutex_lock(&pmus_lock);
	cpumask_set_cpu(cpu, perf_online_mask);
	list_for_each_entry(pmu, &pmus, entry) {
		cpuctx = per_cpu_ptr(pmu->pmu_cpu_context, cpu);
		ctx = &cpuctx->ctx;

		mutex_lock(&ctx->mutex);
		cpuctx->online = 1;
		mutex_unlock(&ctx->mutex);
	}
	mutex_unlock(&pmus_lock);

	return 0;
}