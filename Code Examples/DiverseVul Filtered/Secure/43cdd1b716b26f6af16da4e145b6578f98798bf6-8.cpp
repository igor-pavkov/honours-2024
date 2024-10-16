int acpi_smbus_unregister_callback(struct acpi_smb_hc *hc)
{
	mutex_lock(&hc->lock);
	hc->callback = NULL;
	hc->context = NULL;
	mutex_unlock(&hc->lock);
	return 0;
}