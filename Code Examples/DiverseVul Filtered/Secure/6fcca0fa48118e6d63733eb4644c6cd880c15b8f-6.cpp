static void psi_avgs_work(struct work_struct *work)
{
	struct delayed_work *dwork;
	struct psi_group *group;
	u32 changed_states;
	bool nonidle;
	u64 now;

	dwork = to_delayed_work(work);
	group = container_of(dwork, struct psi_group, avgs_work);

	mutex_lock(&group->avgs_lock);

	now = sched_clock();

	collect_percpu_times(group, PSI_AVGS, &changed_states);
	nonidle = changed_states & (1 << PSI_NONIDLE);
	/*
	 * If there is task activity, periodically fold the per-cpu
	 * times and feed samples into the running averages. If things
	 * are idle and there is no data to process, stop the clock.
	 * Once restarted, we'll catch up the running averages in one
	 * go - see calc_avgs() and missed_periods.
	 */
	if (now >= group->avg_next_update)
		group->avg_next_update = update_averages(group, now);

	if (nonidle) {
		schedule_delayed_work(dwork, nsecs_to_jiffies(
				group->avg_next_update - now) + 1);
	}

	mutex_unlock(&group->avgs_lock);
}