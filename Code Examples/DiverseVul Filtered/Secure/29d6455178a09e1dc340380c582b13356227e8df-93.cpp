static void destroy_sched_domain(struct sched_domain *sd, int cpu)
{
	call_rcu(&sd->rcu, free_sched_domain);
}