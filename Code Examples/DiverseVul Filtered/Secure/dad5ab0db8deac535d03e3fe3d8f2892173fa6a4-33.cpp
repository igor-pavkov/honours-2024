static void acpi_unregister_gsi_ioapic(u32 gsi)
{
#ifdef CONFIG_X86_IO_APIC
	int irq;

	mutex_lock(&acpi_ioapic_lock);
	irq = mp_map_gsi_to_irq(gsi, 0, NULL);
	if (irq > 0)
		mp_unmap_irq(irq);
	mutex_unlock(&acpi_ioapic_lock);
#endif
}