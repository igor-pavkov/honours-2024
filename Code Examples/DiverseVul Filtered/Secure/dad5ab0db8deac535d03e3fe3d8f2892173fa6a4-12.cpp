static int acpi_register_lapic(int id, u32 acpiid, u8 enabled)
{
	unsigned int ver = 0;
	int cpu;

	if (id >= MAX_LOCAL_APIC) {
		printk(KERN_INFO PREFIX "skipped apicid that is too big\n");
		return -EINVAL;
	}

	if (!enabled) {
		++disabled_cpus;
		return -EINVAL;
	}

	if (boot_cpu_physical_apicid != -1U)
		ver = boot_cpu_apic_version;

	cpu = generic_processor_info(id, ver);
	if (cpu >= 0)
		early_per_cpu(x86_cpu_to_acpiid, cpu) = acpiid;

	return cpu;
}