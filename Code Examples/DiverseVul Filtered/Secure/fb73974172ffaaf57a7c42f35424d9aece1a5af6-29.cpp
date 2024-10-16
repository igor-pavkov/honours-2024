static void selinux_perf_event_free(struct perf_event *event)
{
	struct perf_event_security_struct *perfsec = event->security;

	event->security = NULL;
	kfree(perfsec);
}