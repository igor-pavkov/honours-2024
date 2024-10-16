static void free_pmu_context(struct pmu *pmu)
{
	/*
	 * Static contexts such as perf_sw_context have a global lifetime
	 * and may be shared between different PMUs. Avoid freeing them
	 * when a single PMU is going away.
	 */
	if (pmu->task_ctx_nr > perf_invalid_context)
		return;

	free_percpu(pmu->pmu_cpu_context);
}