static void init_triggers(struct psi_group *group, u64 now)
{
	struct psi_trigger *t;

	list_for_each_entry(t, &group->triggers, node)
		window_reset(&t->win, now,
				group->total[PSI_POLL][t->state], 0);
	memcpy(group->polling_total, group->total[PSI_POLL],
		   sizeof(group->polling_total));
	group->polling_next_update = now + group->poll_min_period;
}