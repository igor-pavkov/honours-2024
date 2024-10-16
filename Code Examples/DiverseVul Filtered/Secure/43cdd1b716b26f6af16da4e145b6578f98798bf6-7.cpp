static inline int smb_hc_write(struct acpi_smb_hc *hc, u8 address, u8 data)
{
	return ec_write(hc->offset + address, data);
}