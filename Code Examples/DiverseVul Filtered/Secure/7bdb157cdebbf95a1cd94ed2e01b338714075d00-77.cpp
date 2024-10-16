static enum hrtimer_restart perf_mux_hrtimer_handler(struct hrtimer *hr)
{
	struct perf_cpu_context *cpuctx;
	bool rotations;

	lockdep_assert_irqs_disabled();

	cpuctx = container_of(hr, struct perf_cpu_context, hrtimer);
	rotations = perf_rotate_context(cpuctx);

	raw_spin_lock(&cpuctx->hrtimer_lock);
	if (rotations)
		hrtimer_forward_now(hr, cpuctx->hrtimer_interval);
	else
		cpuctx->hrtimer_active = 0;
	raw_spin_unlock(&cpuctx->hrtimer_lock);

	return rotations ? HRTIMER_RESTART : HRTIMER_NORESTART;
}