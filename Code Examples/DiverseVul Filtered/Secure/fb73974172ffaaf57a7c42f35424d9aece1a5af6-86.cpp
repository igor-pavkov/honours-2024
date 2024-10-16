static int selinux_perf_event_read(struct perf_event *event)
{
	struct perf_event_security_struct *perfsec = event->security;
	u32 sid = current_sid();

	return avc_has_perm(&selinux_state, sid, perfsec->sid,
			    SECCLASS_PERF_EVENT, PERF_EVENT__READ, NULL);
}