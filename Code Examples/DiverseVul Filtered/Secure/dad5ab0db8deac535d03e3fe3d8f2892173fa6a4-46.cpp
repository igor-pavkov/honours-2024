static void __init acpi_sci_ioapic_setup(u8 bus_irq, u16 polarity, u16 trigger, u32 gsi)
{
	if (trigger == 0)	/* compatible SCI trigger is level */
		trigger = 3;

	if (polarity == 0)	/* compatible SCI polarity is low */
		polarity = 3;

	/* Command-line over-ride via acpi_sci= */
	if (acpi_sci_flags & ACPI_MADT_TRIGGER_MASK)
		trigger = (acpi_sci_flags & ACPI_MADT_TRIGGER_MASK) >> 2;

	if (acpi_sci_flags & ACPI_MADT_POLARITY_MASK)
		polarity = acpi_sci_flags & ACPI_MADT_POLARITY_MASK;

	mp_override_legacy_irq(bus_irq, polarity, trigger, gsi);
	acpi_penalize_sci_irq(bus_irq, trigger, polarity);

	/*
	 * stash over-ride to indicate we've been here
	 * and for later update of acpi_gbl_FADT
	 */
	acpi_sci_override_gsi = gsi;
	return;
}