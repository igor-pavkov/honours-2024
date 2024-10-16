static struct psi_group *iterate_groups(struct task_struct *task, void **iter)
{
#ifdef CONFIG_CGROUPS
	struct cgroup *cgroup = NULL;

	if (!*iter)
		cgroup = task->cgroups->dfl_cgrp;
	else if (*iter == &psi_system)
		return NULL;
	else
		cgroup = cgroup_parent(*iter);

	if (cgroup && cgroup_parent(cgroup)) {
		*iter = cgroup;
		return cgroup_psi(cgroup);
	}
#else
	if (*iter)
		return NULL;
#endif
	*iter = &psi_system;
	return &psi_system;
}