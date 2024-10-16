u64 blkg_stat_recursive_sum(struct blkcg_gq *blkg,
			    struct blkcg_policy *pol, int off)
{
	struct blkcg_gq *pos_blkg;
	struct cgroup_subsys_state *pos_css;
	u64 sum = 0;

	lockdep_assert_held(blkg->q->queue_lock);

	rcu_read_lock();
	blkg_for_each_descendant_pre(pos_blkg, pos_css, blkg) {
		struct blkg_stat *stat;

		if (!pos_blkg->online)
			continue;

		if (pol)
			stat = (void *)blkg_to_pd(pos_blkg, pol) + off;
		else
			stat = (void *)blkg + off;

		sum += blkg_stat_read(stat) + atomic64_read(&stat->aux_cnt);
	}
	rcu_read_unlock();

	return sum;
}