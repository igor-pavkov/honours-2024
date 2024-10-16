static int selinux_perf_event_alloc(struct perf_event *event)
{
	struct perf_event_security_struct *perfsec;

	perfsec = kzalloc(sizeof(*perfsec), GFP_KERNEL);
	if (!perfsec)
		return -ENOMEM;

	perfsec->sid = current_sid();
	event->security = perfsec;

	return 0;
}