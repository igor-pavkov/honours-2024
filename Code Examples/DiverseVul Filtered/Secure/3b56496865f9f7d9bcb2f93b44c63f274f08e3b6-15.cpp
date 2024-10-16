static void amd_detect_cmp(struct cpuinfo_x86 *c)
{
#ifdef CONFIG_X86_HT
	unsigned bits;
	int cpu = smp_processor_id();

	bits = c->x86_coreid_bits;
	/* Low order bits define the core id (index of core in socket) */
	c->cpu_core_id = c->initial_apicid & ((1 << bits)-1);
	/* Convert the initial APIC ID into the socket ID */
	c->phys_proc_id = c->initial_apicid >> bits;
	/* use socket ID also for last level cache */
	per_cpu(cpu_llc_id, cpu) = c->phys_proc_id;
	amd_get_topology(c);
#endif
}