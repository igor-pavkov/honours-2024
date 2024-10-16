static inline bool perf_sample_save_hw_index(struct perf_event *event)
{
	return event->attr.branch_sample_type & PERF_SAMPLE_BRANCH_HW_INDEX;
}