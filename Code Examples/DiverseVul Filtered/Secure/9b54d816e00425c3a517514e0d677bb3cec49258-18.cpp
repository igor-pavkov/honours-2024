struct blkg_rwstat blkg_rwstat_recursive_sum(struct blkcg_gq *blkg,
					     struct blkcg_policy *pol, int off)
{
	struct blkcg_gq *pos_blkg;
	struct cgroup_subsys_state *pos_css;
	struct blkg_rwstat sum = { };
	int i;

	lockdep_assert_held(blkg->q->queue_lock);

	rcu_read_lock();
	blkg_for_each_descendant_pre(pos_blkg, pos_css, blkg) {
		struct blkg_rwstat *rwstat;

		if (!pos_blkg->online)
			continue;

		if (pol)
			rwstat = (void *)blkg_to_pd(pos_blkg, pol) + off;
		else
			rwstat = (void *)pos_blkg + off;

		for (i = 0; i < BLKG_RWSTAT_NR; i++)
			atomic64_add(atomic64_read(&rwstat->aux_cnt[i]) +
				percpu_counter_sum_positive(&rwstat->cpu_cnt[i]),
				&sum.aux_cnt[i]);
	}
	rcu_read_unlock();

	return sum;
}