static void *alloc_task_ctx_data(struct pmu *pmu)
{
	if (pmu->task_ctx_cache)
		return kmem_cache_zalloc(pmu->task_ctx_cache, GFP_KERNEL);

	return NULL;
}