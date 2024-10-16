static int perf_event_modify_breakpoint(struct perf_event *bp,
					 struct perf_event_attr *attr)
{
	int err;

	_perf_event_disable(bp);

	err = modify_user_hw_breakpoint_check(bp, attr, true);

	if (!bp->attr.disabled)
		_perf_event_enable(bp);

	return err;
}