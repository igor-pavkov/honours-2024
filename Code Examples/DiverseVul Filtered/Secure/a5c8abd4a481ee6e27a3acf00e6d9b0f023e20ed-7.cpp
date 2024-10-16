void get_global_soft_statistics(struct activity *a, int prev, int curr,
				uint64_t flags, unsigned char offline_cpu_bitmap[])
{
	int i;
	struct stats_softnet *ssnc, *ssnp;
	struct stats_softnet *ssnc_all = (struct stats_softnet *) ((char *) a->buf[curr]);
	struct stats_softnet *ssnp_all = (struct stats_softnet *) ((char *) a->buf[prev]);

	/*
	 * Init structures that will contain values for CPU "all".
	 * CPU "all" doesn't exist in /proc/net/softnet_stat file, so
	 * we compute its values as the sum of the values of each CPU.
	 */
	memset(ssnc_all, 0, sizeof(struct stats_softnet));
	memset(ssnp_all, 0, sizeof(struct stats_softnet));

	for (i = 1; (i < a->nr_ini) && (i < a->bitmap->b_size + 1); i++) {

		/*
		 * The size of a->buf[...] CPU structure may be different from the default
		 * sizeof(struct stats_pwr_cpufreq) value if data have been read from a file!
		 * That's why we don't use a syntax like:
		 * ssnc = (struct stats_softnet *) a->buf[...] + i;
                 */
                ssnc = (struct stats_softnet *) ((char *) a->buf[curr] + i * a->msize);
                ssnp = (struct stats_softnet *) ((char *) a->buf[prev] + i * a->msize);

		if (ssnc->processed + ssnc->dropped + ssnc->time_squeeze +
		    ssnc->received_rps + ssnc->flow_limit == 0) {
			/* Assume current CPU is offline */
			*ssnc = *ssnp;
			offline_cpu_bitmap[i >> 3] |= 1 << (i & 0x07);
		}

		if ((ssnp->processed + ssnp->dropped + ssnp->time_squeeze +
		    ssnp->received_rps + ssnp->flow_limit == 0) && !WANT_SINCE_BOOT(flags)) {
			/* Current CPU back online but no previous sample for it */
			offline_cpu_bitmap[i >> 3] |= 1 << (i & 0x07);
			continue;
		}

		ssnc_all->processed += ssnc->processed;
		ssnc_all->dropped += ssnc->dropped;
		ssnc_all->time_squeeze += ssnc->time_squeeze;
		ssnc_all->received_rps += ssnc->received_rps;
		ssnc_all->flow_limit += ssnc->flow_limit;

		ssnp_all->processed += ssnp->processed;
		ssnp_all->dropped += ssnp->dropped;
		ssnp_all->time_squeeze += ssnp->time_squeeze;
		ssnp_all->received_rps += ssnp->received_rps;
		ssnp_all->flow_limit += ssnp->flow_limit;
	}
}