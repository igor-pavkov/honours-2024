void __init acpi_boot_table_init(void)
{
	dmi_check_system(acpi_dmi_table);

	/*
	 * If acpi_disabled, bail out
	 */
	if (acpi_disabled)
		return;

	/*
	 * Initialize the ACPI boot-time table parser.
	 */
	if (acpi_table_init()) {
		disable_acpi();
		return;
	}

	acpi_table_parse(ACPI_SIG_BOOT, acpi_parse_sbf);

	/*
	 * blacklist may disable ACPI entirely
	 */
	if (acpi_blacklisted()) {
		if (acpi_force) {
			printk(KERN_WARNING PREFIX "acpi=force override\n");
		} else {
			printk(KERN_WARNING PREFIX "Disabling ACPI support\n");
			disable_acpi();
			return;
		}
	}
}