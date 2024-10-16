static int psched_show(struct seq_file *seq, void *v)
{
	struct timespec ts;

	hrtimer_get_res(CLOCK_MONOTONIC, &ts);
	seq_printf(seq, "%08x %08x %08x %08x\n",
		   (u32)NSEC_PER_USEC, (u32)PSCHED_TICKS2NS(1),
		   1000000,
		   (u32)NSEC_PER_SEC/(u32)ktime_to_ns(timespec_to_ktime(ts)));

	return 0;
}