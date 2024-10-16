static void __perf_mux_hrtimer_init(struct perf_cpu_context *cpuctx, int cpu)
{
	struct hrtimer *timer = &cpuctx->hrtimer;
	struct pmu *pmu = cpuctx->ctx.pmu;
	u64 interval;

	/* no multiplexing needed for SW PMU */
	if (pmu->task_ctx_nr == perf_sw_context)
		return;

	/*
	 * check default is sane, if not set then force to
	 * default interval (1/tick)
	 */
	interval = pmu->hrtimer_interval_ms;
	if (interval < 1)
		interval = pmu->hrtimer_interval_ms = PERF_CPU_HRTIMER;

	cpuctx->hrtimer_interval = ns_to_ktime(NSEC_PER_MSEC * interval);

	raw_spin_lock_init(&cpuctx->hrtimer_lock);
	hrtimer_init(timer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS_PINNED_HARD);
	timer->function = perf_mux_hrtimer_handler;
}