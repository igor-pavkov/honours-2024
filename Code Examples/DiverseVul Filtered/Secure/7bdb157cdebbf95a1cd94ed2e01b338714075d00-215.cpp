static int perf_uprobe_event_init(struct perf_event *event)
{
	int err;
	unsigned long ref_ctr_offset;
	bool is_retprobe;

	if (event->attr.type != perf_uprobe.type)
		return -ENOENT;

	if (!perfmon_capable())
		return -EACCES;

	/*
	 * no branch sampling for probe events
	 */
	if (has_branch_stack(event))
		return -EOPNOTSUPP;

	is_retprobe = event->attr.config & PERF_PROBE_CONFIG_IS_RETPROBE;
	ref_ctr_offset = event->attr.config >> PERF_UPROBE_REF_CTR_OFFSET_SHIFT;
	err = perf_uprobe_init(event, ref_ctr_offset, is_retprobe);
	if (err)
		return err;

	event->destroy = perf_uprobe_destroy;

	return 0;
}