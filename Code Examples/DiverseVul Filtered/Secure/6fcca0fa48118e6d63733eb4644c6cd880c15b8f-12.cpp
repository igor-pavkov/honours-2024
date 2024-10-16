void psi_memstall_tick(struct task_struct *task, int cpu)
{
	struct psi_group *group;
	void *iter = NULL;

	while ((group = iterate_groups(task, &iter))) {
		struct psi_group_cpu *groupc;

		groupc = per_cpu_ptr(group->pcpu, cpu);
		write_seqcount_begin(&groupc->seq);
		record_times(groupc, cpu, true);
		write_seqcount_end(&groupc->seq);
	}
}