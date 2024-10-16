static void blkcg_bind(struct cgroup_subsys_state *root_css)
{
	int i;

	mutex_lock(&blkcg_pol_mutex);

	for (i = 0; i < BLKCG_MAX_POLS; i++) {
		struct blkcg_policy *pol = blkcg_policy[i];
		struct blkcg *blkcg;

		if (!pol || !pol->cpd_bind_fn)
			continue;

		list_for_each_entry(blkcg, &all_blkcgs, all_blkcgs_node)
			if (blkcg->cpd[pol->plid])
				pol->cpd_bind_fn(blkcg->cpd[pol->plid]);
	}
	mutex_unlock(&blkcg_pol_mutex);
}