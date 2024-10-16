int acpi_smbus_register_callback(struct acpi_smb_hc *hc,
			         smbus_alarm_callback callback, void *context)
{
	mutex_lock(&hc->lock);
	hc->callback = callback;
	hc->context = context;
	mutex_unlock(&hc->lock);
	return 0;
}