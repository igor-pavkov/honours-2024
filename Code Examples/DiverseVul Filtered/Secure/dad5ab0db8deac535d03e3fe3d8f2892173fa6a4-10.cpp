static int __init acpi_parse_madt_lapic_entries(void)
{
	int count;
	int x2count = 0;
	int ret;
	struct acpi_subtable_proc madt_proc[2];

	if (!boot_cpu_has(X86_FEATURE_APIC))
		return -ENODEV;

	count = acpi_table_parse_madt(ACPI_MADT_TYPE_LOCAL_SAPIC,
				      acpi_parse_sapic, MAX_LOCAL_APIC);

	if (!count) {
		memset(madt_proc, 0, sizeof(madt_proc));
		madt_proc[0].id = ACPI_MADT_TYPE_LOCAL_APIC;
		madt_proc[0].handler = acpi_parse_lapic;
		madt_proc[1].id = ACPI_MADT_TYPE_LOCAL_X2APIC;
		madt_proc[1].handler = acpi_parse_x2apic;
		ret = acpi_table_parse_entries_array(ACPI_SIG_MADT,
				sizeof(struct acpi_table_madt),
				madt_proc, ARRAY_SIZE(madt_proc), MAX_LOCAL_APIC);
		if (ret < 0) {
			printk(KERN_ERR PREFIX
					"Error parsing LAPIC/X2APIC entries\n");
			return ret;
		}

		count = madt_proc[0].count;
		x2count = madt_proc[1].count;
	}
	if (!count && !x2count) {
		printk(KERN_ERR PREFIX "No LAPIC entries present\n");
		/* TBD: Cleanup to allow fallback to MPS */
		return -ENODEV;
	} else if (count < 0 || x2count < 0) {
		printk(KERN_ERR PREFIX "Error parsing LAPIC entry\n");
		/* TBD: Cleanup to allow fallback to MPS */
		return count;
	}

	x2count = acpi_table_parse_madt(ACPI_MADT_TYPE_LOCAL_X2APIC_NMI,
					acpi_parse_x2apic_nmi, 0);
	count = acpi_table_parse_madt(ACPI_MADT_TYPE_LOCAL_APIC_NMI,
				      acpi_parse_lapic_nmi, 0);
	if (count < 0 || x2count < 0) {
		printk(KERN_ERR PREFIX "Error parsing LAPIC NMI entry\n");
		/* TBD: Cleanup to allow fallback to MPS */
		return count;
	}
	return 0;
}