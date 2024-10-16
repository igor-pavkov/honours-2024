static inline int pmu_filter_match(struct perf_event *event)
{
	struct perf_event *sibling;

	if (!__pmu_filter_match(event))
		return 0;

	for_each_sibling_event(sibling, event) {
		if (!__pmu_filter_match(sibling))
			return 0;
	}

	return 1;
}