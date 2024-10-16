static inline int smb_hc_read(struct acpi_smb_hc *hc, u8 address, u8 *data)
{
	return ec_read(hc->offset + address, data);
}