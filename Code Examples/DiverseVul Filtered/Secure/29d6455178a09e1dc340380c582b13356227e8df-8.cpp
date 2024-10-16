static void destroy_sched_domains(struct sched_domain *sd, int cpu)
{
	for (; sd; sd = sd->parent)
		destroy_sched_domain(sd, cpu);
}