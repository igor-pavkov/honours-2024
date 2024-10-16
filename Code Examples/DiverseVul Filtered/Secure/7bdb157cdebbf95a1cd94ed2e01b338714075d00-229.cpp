static inline void update_cgrp_time_from_event(struct perf_event *event)
{
	struct perf_cgroup *cgrp;

	/*
	 * ensure we access cgroup data only when needed and
	 * when we know the cgroup is pinned (css_get)
	 */
	if (!is_cgroup_event(event))
		return;

	cgrp = perf_cgroup_from_task(current, event->ctx);
	/*
	 * Do not update time when cgroup is not active
	 */
	if (cgroup_is_descendant(cgrp->css.cgroup, event->cgrp->css.cgroup))
		__update_cgrp_time(event->cgrp);
}