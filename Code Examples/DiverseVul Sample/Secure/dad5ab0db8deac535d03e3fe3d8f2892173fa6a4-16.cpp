static int acpi_map_cpu2node(acpi_handle handle, int cpu, int physid)
{
#ifdef CONFIG_ACPI_NUMA
	int nid;

	nid = acpi_get_node(handle);
	if (nid != NUMA_NO_NODE) {
		set_apicid_to_node(physid, nid);
		numa_set_node(cpu, nid);
	}
#endif
	return 0;
}