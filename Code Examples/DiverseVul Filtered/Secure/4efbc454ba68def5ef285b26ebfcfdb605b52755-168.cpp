static const struct cpumask *cpu_smt_mask(int cpu)
{
	return topology_thread_cpumask(cpu);
}