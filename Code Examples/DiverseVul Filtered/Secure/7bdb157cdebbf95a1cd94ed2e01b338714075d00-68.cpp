static inline int perf_cgroup_connect(int fd, struct perf_event *event,
				      struct perf_event_attr *attr,
				      struct perf_event *group_leader)
{
	struct perf_cgroup *cgrp;
	struct cgroup_subsys_state *css;
	struct fd f = fdget(fd);
	int ret = 0;

	if (!f.file)
		return -EBADF;

	css = css_tryget_online_from_dir(f.file->f_path.dentry,
					 &perf_event_cgrp_subsys);
	if (IS_ERR(css)) {
		ret = PTR_ERR(css);
		goto out;
	}

	ret = perf_cgroup_ensure_storage(event, css);
	if (ret)
		goto out;

	cgrp = container_of(css, struct perf_cgroup, css);
	event->cgrp = cgrp;

	/*
	 * all events in a group must monitor
	 * the same cgroup because a task belongs
	 * to only one perf cgroup at a time
	 */
	if (group_leader && group_leader->cgrp != cgrp) {
		perf_detach_cgroup(event);
		ret = -EINVAL;
	}
out:
	fdput(f);
	return ret;
}