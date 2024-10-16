static int ftrace_profile_init_cpu(int cpu)
{
	struct ftrace_profile_stat *stat;
	int size;

	stat = &per_cpu(ftrace_profile_stats, cpu);

	if (stat->hash) {
		/* If the profile is already created, simply reset it */
		ftrace_profile_reset(stat);
		return 0;
	}

	/*
	 * We are profiling all functions, but usually only a few thousand
	 * functions are hit. We'll make a hash of 1024 items.
	 */
	size = FTRACE_PROFILE_HASH_SIZE;

	stat->hash = kzalloc(sizeof(struct hlist_head) * size, GFP_KERNEL);

	if (!stat->hash)
		return -ENOMEM;

	if (!ftrace_profile_bits) {
		size--;

		for (; size; size >>= 1)
			ftrace_profile_bits++;
	}

	/* Preallocate the function profiling pages */
	if (ftrace_profile_pages_init(stat) < 0) {
		kfree(stat->hash);
		stat->hash = NULL;
		return -ENOMEM;
	}

	return 0;
}