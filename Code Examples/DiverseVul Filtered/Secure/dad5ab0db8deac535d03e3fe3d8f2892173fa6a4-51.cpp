int acpi_isa_irq_to_gsi(unsigned isa_irq, u32 *gsi)
{
	if (isa_irq < nr_legacy_irqs() &&
	    isa_irq_to_gsi[isa_irq] != ACPI_INVALID_GSI) {
		*gsi = isa_irq_to_gsi[isa_irq];
		return 0;
	}

	return -1;
}