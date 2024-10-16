static void psi_poll_work(struct kthread_work *work)
{
	struct kthread_delayed_work *dwork;
	struct psi_group *group;
	u32 changed_states;
	u64 now;

	dwork = container_of(work, struct kthread_delayed_work, work);
	group = container_of(dwork, struct psi_group, poll_work);

	atomic_set(&group->poll_scheduled, 0);

	mutex_lock(&group->trigger_lock);

	now = sched_clock();

	collect_percpu_times(group, PSI_POLL, &changed_states);

	if (changed_states & group->poll_states) {
		/* Initialize trigger windows when entering polling mode */
		if (now > group->polling_until)
			init_triggers(group, now);

		/*
		 * Keep the monitor active for at least the duration of the
		 * minimum tracking window as long as monitor states are
		 * changing.
		 */
		group->polling_until = now +
			group->poll_min_period * UPDATES_PER_WINDOW;
	}

	if (now > group->polling_until) {
		group->polling_next_update = ULLONG_MAX;
		goto out;
	}

	if (now >= group->polling_next_update)
		group->polling_next_update = update_triggers(group, now);

	psi_schedule_poll_work(group,
		nsecs_to_jiffies(group->polling_next_update - now) + 1);

out:
	mutex_unlock(&group->trigger_lock);
}