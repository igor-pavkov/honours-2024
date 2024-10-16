int __init acpi_boot_init(void)
{
	/* those are executed after early-quirks are executed */
	dmi_check_system(acpi_dmi_table_late);

	/*
	 * If acpi_disabled, bail out
	 */
	if (acpi_disabled)
		return 1;

	acpi_table_parse(ACPI_SIG_BOOT, acpi_parse_sbf);

	/*
	 * set sci_int and PM timer address
	 */
	acpi_table_parse(ACPI_SIG_FADT, acpi_parse_fadt);

	/*
	 * Process the Multiple APIC Description Table (MADT), if present
	 */
	acpi_process_madt();

	acpi_table_parse(ACPI_SIG_HPET, acpi_parse_hpet);
	if (IS_ENABLED(CONFIG_ACPI_BGRT))
		acpi_table_parse(ACPI_SIG_BGRT, acpi_parse_bgrt);

	if (!acpi_noirq)
		x86_init.pci.init = pci_acpi_init;

	return 0;
}