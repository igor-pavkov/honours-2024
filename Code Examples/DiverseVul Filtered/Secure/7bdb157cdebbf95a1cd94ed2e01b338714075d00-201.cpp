int perf_proc_update_handler(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;
	int perf_cpu = sysctl_perf_cpu_time_max_percent;
	/*
	 * If throttling is disabled don't allow the write:
	 */
	if (write && (perf_cpu == 100 || perf_cpu == 0))
		return -EINVAL;

	ret = proc_dointvec_minmax(table, write, buffer, lenp, ppos);
	if (ret || !write)
		return ret;

	max_samples_per_tick = DIV_ROUND_UP(sysctl_perf_event_sample_rate, HZ);
	perf_sample_period_ns = NSEC_PER_SEC / sysctl_perf_event_sample_rate;
	update_perf_cpu_limits();

	return 0;
}