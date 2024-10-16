static void free_pmu_context(struct pmu *pmu)
{
	mutex_lock(&pmus_lock);
	free_percpu(pmu->pmu_cpu_context);
	mutex_unlock(&pmus_lock);
}