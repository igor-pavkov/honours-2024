void perf_event_fork(struct task_struct *task)
{
	perf_event_task(task, NULL, 1);
	perf_event_namespaces(task);
}