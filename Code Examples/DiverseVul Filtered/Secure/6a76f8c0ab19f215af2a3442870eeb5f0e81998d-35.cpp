static void clear_ftrace_swapper(void)
{
	struct task_struct *p;
	int cpu;

	get_online_cpus();
	for_each_online_cpu(cpu) {
		p = idle_task(cpu);
		clear_tsk_trace_trace(p);
	}
	put_online_cpus();
}