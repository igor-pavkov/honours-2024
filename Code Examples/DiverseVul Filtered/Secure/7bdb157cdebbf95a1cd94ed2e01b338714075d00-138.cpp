static int perf_cgroup_ensure_storage(struct perf_event *event,
				struct cgroup_subsys_state *css)
{
	struct perf_cpu_context *cpuctx;
	struct perf_event **storage;
	int cpu, heap_size, ret = 0;

	/*
	 * Allow storage to have sufficent space for an iterator for each
	 * possibly nested cgroup plus an iterator for events with no cgroup.
	 */
	for (heap_size = 1; css; css = css->parent)
		heap_size++;

	for_each_possible_cpu(cpu) {
		cpuctx = per_cpu_ptr(event->pmu->pmu_cpu_context, cpu);
		if (heap_size <= cpuctx->heap_size)
			continue;

		storage = kmalloc_node(heap_size * sizeof(struct perf_event *),
				       GFP_KERNEL, cpu_to_node(cpu));
		if (!storage) {
			ret = -ENOMEM;
			break;
		}

		raw_spin_lock_irq(&cpuctx->ctx.lock);
		if (cpuctx->heap_size < heap_size) {
			swap(cpuctx->heap, storage);
			if (storage == cpuctx->heap_default)
				storage = NULL;
			cpuctx->heap_size = heap_size;
		}
		raw_spin_unlock_irq(&cpuctx->ctx.lock);

		kfree(storage);
	}

	return ret;
}