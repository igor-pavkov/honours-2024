static int perf_cgroup_css_online(struct cgroup_subsys_state *css)
{
	perf_event_cgroup(css->cgroup);
	return 0;
}