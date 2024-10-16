int psi_cgroup_alloc(struct cgroup *cgroup)
{
	if (static_branch_likely(&psi_disabled))
		return 0;

	cgroup->psi.pcpu = alloc_percpu(struct psi_group_cpu);
	if (!cgroup->psi.pcpu)
		return -ENOMEM;
	group_init(&cgroup->psi);
	return 0;
}