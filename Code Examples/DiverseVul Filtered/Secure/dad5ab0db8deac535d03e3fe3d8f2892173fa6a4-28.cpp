static void __init acpi_set_irq_model_ioapic(void)
{
	acpi_irq_model = ACPI_IRQ_MODEL_IOAPIC;
	__acpi_register_gsi = acpi_register_gsi_ioapic;
	__acpi_unregister_gsi = acpi_unregister_gsi_ioapic;
	acpi_ioapic = 1;
}