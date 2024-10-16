static noinline int visit_groups_merge(struct perf_cpu_context *cpuctx,
				struct perf_event_groups *groups, int cpu,
				int (*func)(struct perf_event *, void *),
				void *data)
{
#ifdef CONFIG_CGROUP_PERF
	struct cgroup_subsys_state *css = NULL;
#endif
	/* Space for per CPU and/or any CPU event iterators. */
	struct perf_event *itrs[2];
	struct min_heap event_heap;
	struct perf_event **evt;
	int ret;

	if (cpuctx) {
		event_heap = (struct min_heap){
			.data = cpuctx->heap,
			.nr = 0,
			.size = cpuctx->heap_size,
		};

		lockdep_assert_held(&cpuctx->ctx.lock);

#ifdef CONFIG_CGROUP_PERF
		if (cpuctx->cgrp)
			css = &cpuctx->cgrp->css;
#endif
	} else {
		event_heap = (struct min_heap){
			.data = itrs,
			.nr = 0,
			.size = ARRAY_SIZE(itrs),
		};
		/* Events not within a CPU context may be on any CPU. */
		__heap_add(&event_heap, perf_event_groups_first(groups, -1, NULL));
	}
	evt = event_heap.data;

	__heap_add(&event_heap, perf_event_groups_first(groups, cpu, NULL));

#ifdef CONFIG_CGROUP_PERF
	for (; css; css = css->parent)
		__heap_add(&event_heap, perf_event_groups_first(groups, cpu, css->cgroup));
#endif

	min_heapify_all(&event_heap, &perf_min_heap);

	while (event_heap.nr) {
		ret = func(*evt, data);
		if (ret)
			return ret;

		*evt = perf_event_groups_next(*evt);
		if (*evt)
			min_heapify(&event_heap, 0, &perf_min_heap);
		else
			min_heap_pop(&event_heap, &perf_min_heap);
	}

	return 0;
}