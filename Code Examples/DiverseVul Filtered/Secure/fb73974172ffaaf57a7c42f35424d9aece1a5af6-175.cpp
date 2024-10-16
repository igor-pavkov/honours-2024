static int selinux_perf_event_open(struct perf_event_attr *attr, int type)
{
	u32 requested, sid = current_sid();

	if (type == PERF_SECURITY_OPEN)
		requested = PERF_EVENT__OPEN;
	else if (type == PERF_SECURITY_CPU)
		requested = PERF_EVENT__CPU;
	else if (type == PERF_SECURITY_KERNEL)
		requested = PERF_EVENT__KERNEL;
	else if (type == PERF_SECURITY_TRACEPOINT)
		requested = PERF_EVENT__TRACEPOINT;
	else
		return -EINVAL;

	return avc_has_perm(&selinux_state, sid, sid, SECCLASS_PERF_EVENT,
			    requested, NULL);
}