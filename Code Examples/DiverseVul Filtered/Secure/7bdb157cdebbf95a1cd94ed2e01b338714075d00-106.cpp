static int perf_mux_hrtimer_restart(struct perf_cpu_context *cpuctx)
{
	struct hrtimer *timer = &cpuctx->hrtimer;
	struct pmu *pmu = cpuctx->ctx.pmu;
	unsigned long flags;

	/* not for SW PMU */
	if (pmu->task_ctx_nr == perf_sw_context)
		return 0;

	raw_spin_lock_irqsave(&cpuctx->hrtimer_lock, flags);
	if (!cpuctx->hrtimer_active) {
		cpuctx->hrtimer_active = 1;
		hrtimer_forward_now(timer, cpuctx->hrtimer_interval);
		hrtimer_start_expires(timer, HRTIMER_MODE_ABS_PINNED_HARD);
	}
	raw_spin_unlock_irqrestore(&cpuctx->hrtimer_lock, flags);

	return 0;
}