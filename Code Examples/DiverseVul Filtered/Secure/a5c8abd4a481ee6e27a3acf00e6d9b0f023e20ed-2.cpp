unsigned long long get_global_cpu_statistics(struct activity *a, int prev, int curr,
					     uint64_t flags, unsigned char offline_cpu_bitmap[])
{
	int i;
	unsigned long long tot_jiffies_c, tot_jiffies_p;
	unsigned long long deltot_jiffies = 0;
	struct stats_cpu *scc, *scp;
	struct stats_cpu *scc_all = (struct stats_cpu *) ((char *) a->buf[curr]);
	struct stats_cpu *scp_all = (struct stats_cpu *) ((char *) a->buf[prev]);

	/*
	 * Initial processing.
	 * Compute CPU "all" as sum of all individual CPU. Done only on SMP machines (a->nr_ini > 1).
	 * For UP machines we keep the values read from global CPU line in /proc/stat.
	 * Also look for offline CPU: They won't be displayed, and some of their values may
	 * have to be modified.
	 */
	if (a->nr_ini > 1) {
		memset(scc_all, 0, sizeof(struct stats_cpu));
		memset(scp_all, 0, sizeof(struct stats_cpu));
	}

	for (i = 1; (i < a->nr_ini) && (i < a->bitmap->b_size + 1); i++) {

		/*
		 * The size of a->buf[...] CPU structure may be different from the default
		 * sizeof(struct stats_cpu) value if data have been read from a file!
		 * That's why we don't use a syntax like:
		 * scc = (struct stats_cpu *) a->buf[...] + i;
		 */
		scc = (struct stats_cpu *) ((char *) a->buf[curr] + i * a->msize);
		scp = (struct stats_cpu *) ((char *) a->buf[prev] + i * a->msize);

		/*
		 * Compute the total number of jiffies spent by current processor.
		 * NB: Don't add cpu_guest/cpu_guest_nice because cpu_user/cpu_nice
		 * already include them.
		 */
		tot_jiffies_c = scc->cpu_user + scc->cpu_nice +
				scc->cpu_sys + scc->cpu_idle +
				scc->cpu_iowait + scc->cpu_hardirq +
				scc->cpu_steal + scc->cpu_softirq;
		tot_jiffies_p = scp->cpu_user + scp->cpu_nice +
				scp->cpu_sys + scp->cpu_idle +
				scp->cpu_iowait + scp->cpu_hardirq +
				scp->cpu_steal + scp->cpu_softirq;

		/*
		 * If the CPU is offline then it is omited from /proc/stat:
		 * All the fields couldn't have been read and the sum of them is zero.
		 */
		if (tot_jiffies_c == 0) {
			/*
			 * CPU is currently offline.
			 * Set current struct fields (which have been set to zero)
			 * to values from previous iteration. Hence their values won't
			 * jump from zero when the CPU comes back online.
			 * Note that this workaround no longer fully applies with recent kernels,
			 * as I have noticed that when a CPU comes back online, some fields
			 * restart from their previous value (e.g. user, nice, system)
			 * whereas others restart from zero (idle, iowait)! To deal with this,
			 * the get_per_cpu_interval() function will set these previous values
			 * to zero if necessary.
			 */
			*scc = *scp;

			/*
			 * Mark CPU as offline to not display it
			 * (and thus it will not be confused with a tickless CPU).
			 */
			offline_cpu_bitmap[i >> 3] |= 1 << (i & 0x07);
		}

		if ((tot_jiffies_p == 0) && !WANT_SINCE_BOOT(flags)) {
			/*
			 * CPU has just come back online.
			 * Unfortunately, no reference values are available
			 * from a previous iteration, probably because it was
			 * already offline when the first sample has been taken.
			 * So don't display that CPU to prevent "jump-from-zero"
			 * output syndrome, and don't take it into account for CPU "all".
			 */
			offline_cpu_bitmap[i >> 3] |= 1 << (i & 0x07);
			continue;
		}

		/*
		 * Get interval for current CPU and add it to global CPU.
		 * Note: Previous idle and iowait values (saved in scp) may be modified here.
		 */
		deltot_jiffies += get_per_cpu_interval(scc, scp);

		scc_all->cpu_user += scc->cpu_user;
		scp_all->cpu_user += scp->cpu_user;

		scc_all->cpu_nice += scc->cpu_nice;
		scp_all->cpu_nice += scp->cpu_nice;

		scc_all->cpu_sys += scc->cpu_sys;
		scp_all->cpu_sys += scp->cpu_sys;

		scc_all->cpu_idle += scc->cpu_idle;
		scp_all->cpu_idle += scp->cpu_idle;

		scc_all->cpu_iowait += scc->cpu_iowait;
		scp_all->cpu_iowait += scp->cpu_iowait;

		scc_all->cpu_hardirq += scc->cpu_hardirq;
		scp_all->cpu_hardirq += scp->cpu_hardirq;

		scc_all->cpu_steal += scc->cpu_steal;
		scp_all->cpu_steal += scp->cpu_steal;

		scc_all->cpu_softirq += scc->cpu_softirq;
		scp_all->cpu_softirq += scp->cpu_softirq;

		scc_all->cpu_guest += scc->cpu_guest;
		scp_all->cpu_guest += scp->cpu_guest;

		scc_all->cpu_guest_nice += scc->cpu_guest_nice;
		scp_all->cpu_guest_nice += scp->cpu_guest_nice;
	}

	return deltot_jiffies;
}