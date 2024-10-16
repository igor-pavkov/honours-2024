int psi_show(struct seq_file *m, struct psi_group *group, enum psi_res res)
{
	int full;
	u64 now;

	if (static_branch_likely(&psi_disabled))
		return -EOPNOTSUPP;

	/* Update averages before reporting them */
	mutex_lock(&group->avgs_lock);
	now = sched_clock();
	collect_percpu_times(group, PSI_AVGS, NULL);
	if (now >= group->avg_next_update)
		group->avg_next_update = update_averages(group, now);
	mutex_unlock(&group->avgs_lock);

	for (full = 0; full < 2 - (res == PSI_CPU); full++) {
		unsigned long avg[3];
		u64 total;
		int w;

		for (w = 0; w < 3; w++)
			avg[w] = group->avg[res * 2 + full][w];
		total = div_u64(group->total[PSI_AVGS][res * 2 + full],
				NSEC_PER_USEC);

		seq_printf(m, "%s avg10=%lu.%02lu avg60=%lu.%02lu avg300=%lu.%02lu total=%llu\n",
			   full ? "full" : "some",
			   LOAD_INT(avg[0]), LOAD_FRAC(avg[0]),
			   LOAD_INT(avg[1]), LOAD_FRAC(avg[1]),
			   LOAD_INT(avg[2]), LOAD_FRAC(avg[2]),
			   total);
	}

	return 0;
}