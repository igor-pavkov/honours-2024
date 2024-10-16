static void free_task_ctx_data(struct pmu *pmu, void *task_ctx_data)
{
	if (pmu->task_ctx_cache && task_ctx_data)
		kmem_cache_free(pmu->task_ctx_cache, task_ctx_data);
}