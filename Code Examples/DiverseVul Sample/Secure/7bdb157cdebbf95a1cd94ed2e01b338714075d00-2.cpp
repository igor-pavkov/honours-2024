static bool perf_less_group_idx(const void *l, const void *r)
{
	const struct perf_event *le = *(const struct perf_event **)l;
	const struct perf_event *re = *(const struct perf_event **)r;

	return le->group_index < re->group_index;
}