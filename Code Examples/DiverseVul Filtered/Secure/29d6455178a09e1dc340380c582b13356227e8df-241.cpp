static int cpu_shares_write_u64(struct cgroup_subsys_state *css,
				struct cftype *cftype, u64 shareval)
{
	return sched_group_set_shares(css_tg(css), scale_load(shareval));
}