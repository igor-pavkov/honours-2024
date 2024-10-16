int group_balance_cpu(struct sched_group *sg)
{
	return cpumask_first_and(sched_group_cpus(sg), sched_group_mask(sg));
}