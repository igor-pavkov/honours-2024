static u64 update_triggers(struct psi_group *group, u64 now)
{
	struct psi_trigger *t;
	bool new_stall = false;
	u64 *total = group->total[PSI_POLL];

	/*
	 * On subsequent updates, calculate growth deltas and let
	 * watchers know when their specified thresholds are exceeded.
	 */
	list_for_each_entry(t, &group->triggers, node) {
		u64 growth;

		/* Check for stall activity */
		if (group->polling_total[t->state] == total[t->state])
			continue;

		/*
		 * Multiple triggers might be looking at the same state,
		 * remember to update group->polling_total[] once we've
		 * been through all of them. Also remember to extend the
		 * polling time if we see new stall activity.
		 */
		new_stall = true;

		/* Calculate growth since last update */
		growth = window_update(&t->win, now, total[t->state]);
		if (growth < t->threshold)
			continue;

		/* Limit event signaling to once per window */
		if (now < t->last_event_time + t->win.size)
			continue;

		/* Generate an event */
		if (cmpxchg(&t->event, 0, 1) == 0)
			wake_up_interruptible(&t->event_wait);
		t->last_event_time = now;
	}

	if (new_stall)
		memcpy(group->polling_total, total,
				sizeof(group->polling_total));

	return now + group->poll_min_period;
}