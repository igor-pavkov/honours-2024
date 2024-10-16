static inline void acpi_smbus_callback(void *context)
{
	struct acpi_smb_hc *hc = context;
	if (hc->callback)
		hc->callback(hc->context);
}