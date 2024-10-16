int __init early_acpi_boot_init(void)
{
	/*
	 * If acpi_disabled, bail out
	 */
	if (acpi_disabled)
		return 1;

	/*
	 * Process the Multiple APIC Description Table (MADT), if present
	 */
	early_acpi_process_madt();

	/*
	 * Hardware-reduced ACPI mode initialization:
	 */
	acpi_reduced_hw_init();

	return 0;
}