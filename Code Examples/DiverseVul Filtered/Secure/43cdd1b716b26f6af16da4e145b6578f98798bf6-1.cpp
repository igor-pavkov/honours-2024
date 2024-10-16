int acpi_smbus_read(struct acpi_smb_hc *hc, u8 protocol, u8 address,
		    u8 command, u8 *data)
{
	return acpi_smbus_transaction(hc, protocol, address, command, data, 0);
}