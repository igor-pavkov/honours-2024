static int perf_get_aux_event(struct perf_event *event,
			      struct perf_event *group_leader)
{
	/*
	 * Our group leader must be an aux event if we want to be
	 * an aux_output. This way, the aux event will precede its
	 * aux_output events in the group, and therefore will always
	 * schedule first.
	 */
	if (!group_leader)
		return 0;

	/*
	 * aux_output and aux_sample_size are mutually exclusive.
	 */
	if (event->attr.aux_output && event->attr.aux_sample_size)
		return 0;

	if (event->attr.aux_output &&
	    !perf_aux_output_match(event, group_leader))
		return 0;

	if (event->attr.aux_sample_size && !group_leader->pmu->snapshot_aux)
		return 0;

	if (!atomic_long_inc_not_zero(&group_leader->refcount))
		return 0;

	/*
	 * Link aux_outputs to their aux event; this is undone in
	 * perf_group_detach() by perf_put_aux_event(). When the
	 * group in torn down, the aux_output events loose their
	 * link to the aux_event and can't schedule any more.
	 */
	event->aux_event = group_leader;

	return 1;
}