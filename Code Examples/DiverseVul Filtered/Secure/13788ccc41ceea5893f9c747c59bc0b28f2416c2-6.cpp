static void hrtimer_get_softirq_time(struct hrtimer_cpu_base *base)
{
	ktime_t xtim, tomono;
	struct timespec xts;
	unsigned long seq;

	do {
		seq = read_seqbegin(&xtime_lock);
#ifdef CONFIG_NO_HZ
		getnstimeofday(&xts);
#else
		xts = xtime;
#endif
	} while (read_seqretry(&xtime_lock, seq));

	xtim = timespec_to_ktime(xts);
	tomono = timespec_to_ktime(wall_to_monotonic);
	base->clock_base[CLOCK_REALTIME].softirq_time = xtim;
	base->clock_base[CLOCK_MONOTONIC].softirq_time =
		ktime_add(xtim, tomono);
}