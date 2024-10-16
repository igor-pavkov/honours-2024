static void perf_event_exit_cpu_context(int cpu)
{
	struct perf_cpu_context *cpuctx;
	struct perf_event_context *ctx;
	struct pmu *pmu;

	mutex_lock(&pmus_lock);
	list_for_each_entry(pmu, &pmus, entry) {
		cpuctx = per_cpu_ptr(pmu->pmu_cpu_context, cpu);
		ctx = &cpuctx->ctx;

		mutex_lock(&ctx->mutex);
		smp_call_function_single(cpu, __perf_event_exit_context, ctx, 1);
		cpuctx->online = 0;
		mutex_unlock(&ctx->mutex);
	}
	cpumask_clear_cpu(cpu, perf_online_mask);
	mutex_unlock(&pmus_lock);
}