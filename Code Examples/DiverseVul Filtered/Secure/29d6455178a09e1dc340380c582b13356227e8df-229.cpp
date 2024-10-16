static int sched_cpu_active(struct notifier_block *nfb,
				      unsigned long action, void *hcpu)
{
	int cpu = (long)hcpu;

	switch (action & ~CPU_TASKS_FROZEN) {
	case CPU_STARTING:
		set_cpu_rq_start_time();
		return NOTIFY_OK;

	case CPU_DOWN_FAILED:
		set_cpu_active(cpu, true);
		return NOTIFY_OK;

	default:
		return NOTIFY_DONE;
	}
}