static u32 perf_event_tid(struct perf_event *event, struct task_struct *p)
{
	return perf_event_pid_type(event, p, PIDTYPE_PID);
}