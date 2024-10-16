static void update_pvclock_gtod(struct timekeeper *tk)
{
	struct pvclock_gtod_data *vdata = &pvclock_gtod_data;

	write_seqcount_begin(&vdata->seq);

	/* copy pvclock gtod data */
	vdata->clock.vclock_mode	= tk->clock->archdata.vclock_mode;
	vdata->clock.cycle_last		= tk->clock->cycle_last;
	vdata->clock.mask		= tk->clock->mask;
	vdata->clock.mult		= tk->mult;
	vdata->clock.shift		= tk->shift;

	vdata->monotonic_time_sec	= tk->xtime_sec
					+ tk->wall_to_monotonic.tv_sec;
	vdata->monotonic_time_snsec	= tk->xtime_nsec
					+ (tk->wall_to_monotonic.tv_nsec
						<< tk->shift);
	while (vdata->monotonic_time_snsec >=
					(((u64)NSEC_PER_SEC) << tk->shift)) {
		vdata->monotonic_time_snsec -=
					((u64)NSEC_PER_SEC) << tk->shift;
		vdata->monotonic_time_sec++;
	}

	write_seqcount_end(&vdata->seq);
}