static inline void update_cgrp_time_from_cpuctx(struct perf_cpu_context *cpuctx)
{
	struct perf_cgroup *cgrp = cpuctx->cgrp;
	struct cgroup_subsys_state *css;

	if (cgrp) {
		for (css = &cgrp->css; css; css = css->parent) {
			cgrp = container_of(css, struct perf_cgroup, css);
			__update_cgrp_time(cgrp);
		}
	}
}