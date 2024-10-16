static void perf_output_read_group(struct perf_output_handle *handle,
			    struct perf_event *event,
			    u64 enabled, u64 running)
{
	struct perf_event *leader = event->group_leader, *sub;
	u64 read_format = event->attr.read_format;
	u64 values[5];
	int n = 0;

	values[n++] = 1 + leader->nr_siblings;

	if (read_format & PERF_FORMAT_TOTAL_TIME_ENABLED)
		values[n++] = enabled;

	if (read_format & PERF_FORMAT_TOTAL_TIME_RUNNING)
		values[n++] = running;

	if ((leader != event) &&
	    (leader->state == PERF_EVENT_STATE_ACTIVE))
		leader->pmu->read(leader);

	values[n++] = perf_event_count(leader);
	if (read_format & PERF_FORMAT_ID)
		values[n++] = primary_event_id(leader);

	__output_copy(handle, values, n * sizeof(u64));

	for_each_sibling_event(sub, leader) {
		n = 0;

		if ((sub != event) &&
		    (sub->state == PERF_EVENT_STATE_ACTIVE))
			sub->pmu->read(sub);

		values[n++] = perf_event_count(sub);
		if (read_format & PERF_FORMAT_ID)
			values[n++] = primary_event_id(sub);

		__output_copy(handle, values, n * sizeof(u64));
	}
}