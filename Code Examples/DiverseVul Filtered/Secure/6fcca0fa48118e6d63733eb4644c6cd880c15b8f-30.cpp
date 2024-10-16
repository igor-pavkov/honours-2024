static void record_times(struct psi_group_cpu *groupc, int cpu,
			 bool memstall_tick)
{
	u32 delta;
	u64 now;

	now = cpu_clock(cpu);
	delta = now - groupc->state_start;
	groupc->state_start = now;

	if (groupc->state_mask & (1 << PSI_IO_SOME)) {
		groupc->times[PSI_IO_SOME] += delta;
		if (groupc->state_mask & (1 << PSI_IO_FULL))
			groupc->times[PSI_IO_FULL] += delta;
	}

	if (groupc->state_mask & (1 << PSI_MEM_SOME)) {
		groupc->times[PSI_MEM_SOME] += delta;
		if (groupc->state_mask & (1 << PSI_MEM_FULL))
			groupc->times[PSI_MEM_FULL] += delta;
		else if (memstall_tick) {
			u32 sample;
			/*
			 * Since we care about lost potential, a
			 * memstall is FULL when there are no other
			 * working tasks, but also when the CPU is
			 * actively reclaiming and nothing productive
			 * could run even if it were runnable.
			 *
			 * When the timer tick sees a reclaiming CPU,
			 * regardless of runnable tasks, sample a FULL
			 * tick (or less if it hasn't been a full tick
			 * since the last state change).
			 */
			sample = min(delta, (u32)jiffies_to_nsecs(1));
			groupc->times[PSI_MEM_FULL] += sample;
		}
	}

	if (groupc->state_mask & (1 << PSI_CPU_SOME))
		groupc->times[PSI_CPU_SOME] += delta;

	if (groupc->state_mask & (1 << PSI_NONIDLE))
		groupc->times[PSI_NONIDLE] += delta;
}