static int acpi_smbus_hc_remove(struct acpi_device *device)
{
	struct acpi_smb_hc *hc;

	if (!device)
		return -EINVAL;

	hc = acpi_driver_data(device);
	acpi_ec_remove_query_handler(hc->ec, hc->query_bit);
	kfree(hc);
	device->driver_data = NULL;
	return 0;
}