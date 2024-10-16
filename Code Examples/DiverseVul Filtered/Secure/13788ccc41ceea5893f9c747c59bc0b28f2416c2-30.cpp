static void retrigger_next_event(void *arg)
{
	struct hrtimer_cpu_base *base;
	struct timespec realtime_offset;
	unsigned long seq;

	if (!hrtimer_hres_active())
		return;

	do {
		seq = read_seqbegin(&xtime_lock);
		set_normalized_timespec(&realtime_offset,
					-wall_to_monotonic.tv_sec,
					-wall_to_monotonic.tv_nsec);
	} while (read_seqretry(&xtime_lock, seq));

	base = &__get_cpu_var(hrtimer_bases);

	/* Adjust CLOCK_REALTIME offset */
	spin_lock(&base->lock);
	base->clock_base[CLOCK_REALTIME].offset =
		timespec_to_ktime(realtime_offset);

	hrtimer_force_reprogram(base);
	spin_unlock(&base->lock);
}