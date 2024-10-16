int acpi_unmap_cpu(int cpu)
{
#ifdef CONFIG_ACPI_NUMA
	set_apicid_to_node(per_cpu(x86_cpu_to_apicid, cpu), NUMA_NO_NODE);
#endif

	per_cpu(x86_cpu_to_apicid, cpu) = -1;
	set_cpu_present(cpu, false);
	num_processors--;

	return (0);
}