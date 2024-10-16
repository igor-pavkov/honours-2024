int acpi_ioapic_registered(acpi_handle handle, u32 gsi_base)
{
	int ret = 0;

#ifdef CONFIG_ACPI_HOTPLUG_IOAPIC
	mutex_lock(&acpi_ioapic_lock);
	ret  = mp_ioapic_registered(gsi_base);
	mutex_unlock(&acpi_ioapic_lock);
#endif

	return ret;
}