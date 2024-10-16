int acpi_map_cpu(acpi_handle handle, phys_cpuid_t physid, u32 acpi_id,
		 int *pcpu)
{
	int cpu;

	cpu = acpi_register_lapic(physid, acpi_id, ACPI_MADT_ENABLED);
	if (cpu < 0) {
		pr_info(PREFIX "Unable to map lapic to logical cpu number\n");
		return cpu;
	}

	acpi_processor_set_pdc(handle);
	acpi_map_cpu2node(handle, cpu, physid);

	*pcpu = cpu;
	return 0;
}