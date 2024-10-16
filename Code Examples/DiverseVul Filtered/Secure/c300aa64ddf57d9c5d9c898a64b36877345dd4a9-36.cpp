static int do_monotonic(struct timespec *ts, cycle_t *cycle_now)
{
	unsigned long seq;
	u64 ns;
	int mode;
	struct pvclock_gtod_data *gtod = &pvclock_gtod_data;

	ts->tv_nsec = 0;
	do {
		seq = read_seqcount_begin(&gtod->seq);
		mode = gtod->clock.vclock_mode;
		ts->tv_sec = gtod->monotonic_time_sec;
		ns = gtod->monotonic_time_snsec;
		ns += vgettsc(cycle_now);
		ns >>= gtod->clock.shift;
	} while (unlikely(read_seqcount_retry(&gtod->seq, seq)));
	timespec_add_ns(ts, ns);

	return mode;
}