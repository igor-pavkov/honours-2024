static void free_sched_domain(struct rcu_head *rcu)
{
	struct sched_domain *sd = container_of(rcu, struct sched_domain, rcu);

	/*
	 * If its an overlapping domain it has private groups, iterate and
	 * nuke them all.
	 */
	if (sd->flags & SD_OVERLAP) {
		free_sched_groups(sd->groups, 1);
	} else if (atomic_dec_and_test(&sd->groups->ref)) {
		kfree(sd->groups->sgc);
		kfree(sd->groups);
	}
	kfree(sd);
}