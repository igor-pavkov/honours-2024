static int __init acpi_parse_madt(struct acpi_table_header *table)
{
	struct acpi_table_madt *madt = NULL;

	if (!boot_cpu_has(X86_FEATURE_APIC))
		return -EINVAL;

	madt = (struct acpi_table_madt *)table;
	if (!madt) {
		printk(KERN_WARNING PREFIX "Unable to map MADT\n");
		return -ENODEV;
	}

	if (madt->address) {
		acpi_lapic_addr = (u64) madt->address;

		printk(KERN_DEBUG PREFIX "Local APIC address 0x%08x\n",
		       madt->address);
	}

	default_acpi_madt_oem_check(madt->header.oem_id,
				    madt->header.oem_table_id);

	return 0;
}