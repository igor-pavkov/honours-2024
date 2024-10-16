int cgroup_transfer_tasks(struct cgroup *to, struct cgroup *from)
{
	DEFINE_CGROUP_MGCTX(mgctx);
	struct cgrp_cset_link *link;
	struct css_task_iter it;
	struct task_struct *task;
	int ret;

	if (cgroup_on_dfl(to))
		return -EINVAL;

	ret = cgroup_migrate_vet_dst(to);
	if (ret)
		return ret;

	mutex_lock(&cgroup_mutex);

	percpu_down_write(&cgroup_threadgroup_rwsem);

	/* all tasks in @from are being moved, all csets are source */
	spin_lock_irq(&css_set_lock);
	list_for_each_entry(link, &from->cset_links, cset_link)
		cgroup_migrate_add_src(link->cset, to, &mgctx);
	spin_unlock_irq(&css_set_lock);

	ret = cgroup_migrate_prepare_dst(&mgctx);
	if (ret)
		goto out_err;

	/*
	 * Migrate tasks one-by-one until @from is empty.  This fails iff
	 * ->can_attach() fails.
	 */
	do {
		css_task_iter_start(&from->self, 0, &it);

		do {
			task = css_task_iter_next(&it);
		} while (task && (task->flags & PF_EXITING));

		if (task)
			get_task_struct(task);
		css_task_iter_end(&it);

		if (task) {
			ret = cgroup_migrate(task, false, &mgctx);
			if (!ret)
				TRACE_CGROUP_PATH(transfer_tasks, to, task, false);
			put_task_struct(task);
		}
	} while (task && !ret);
out_err:
	cgroup_migrate_finish(&mgctx);
	percpu_up_write(&cgroup_threadgroup_rwsem);
	mutex_unlock(&cgroup_mutex);
	return ret;
}