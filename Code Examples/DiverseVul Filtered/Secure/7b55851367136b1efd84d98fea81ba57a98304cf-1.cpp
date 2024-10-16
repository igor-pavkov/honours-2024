static inline void init_task_pid_links(struct task_struct *task)
{
	enum pid_type type;

	for (type = PIDTYPE_PID; type < PIDTYPE_MAX; ++type) {
		INIT_HLIST_NODE(&task->pid_links[type]);
	}
}