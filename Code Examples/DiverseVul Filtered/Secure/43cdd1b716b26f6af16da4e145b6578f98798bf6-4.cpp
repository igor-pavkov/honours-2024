int acpi_smbus_write(struct acpi_smb_hc *hc, u8 protocol, u8 address,
		     u8 command, u8 *data, u8 length)
{
	return acpi_smbus_transaction(hc, protocol, address, command, data, length);
}