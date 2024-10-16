void acpi_unregister_gsi(u32 gsi)
{
	if (__acpi_unregister_gsi)
		__acpi_unregister_gsi(gsi);
}