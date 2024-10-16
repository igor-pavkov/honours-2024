static int perf_event_modify_attr(struct perf_event *event,
				  struct perf_event_attr *attr)
{
	if (event->attr.type != attr->type)
		return -EINVAL;

	switch (event->attr.type) {
	case PERF_TYPE_BREAKPOINT:
		return perf_event_modify_breakpoint(event, attr);
	default:
		/* Place holder for future additions. */
		return -EOPNOTSUPP;
	}
}