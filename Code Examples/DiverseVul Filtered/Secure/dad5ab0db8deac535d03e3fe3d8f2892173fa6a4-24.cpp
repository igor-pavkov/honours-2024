int acpi_unregister_ioapic(acpi_handle handle, u32 gsi_base)
{
	int ret = -ENOSYS;

#ifdef CONFIG_ACPI_HOTPLUG_IOAPIC
	mutex_lock(&acpi_ioapic_lock);
	ret  = mp_unregister_ioapic(gsi_base);
	mutex_unlock(&acpi_ioapic_lock);
#endif

	return ret;
}