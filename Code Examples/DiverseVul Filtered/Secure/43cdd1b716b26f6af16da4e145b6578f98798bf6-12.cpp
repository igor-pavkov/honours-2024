static int acpi_smbus_transaction(struct acpi_smb_hc *hc, u8 protocol,
				  u8 address, u8 command, u8 *data, u8 length)
{
	int ret = -EFAULT, i;
	u8 temp, sz = 0;

	if (!hc) {
		printk(KERN_ERR PREFIX "host controller is not configured\n");
		return ret;
	}

	mutex_lock(&hc->lock);
	hc->done = false;
	if (smb_hc_read(hc, ACPI_SMB_PROTOCOL, &temp))
		goto end;
	if (temp) {
		ret = -EBUSY;
		goto end;
	}
	smb_hc_write(hc, ACPI_SMB_COMMAND, command);
	if (!(protocol & 0x01)) {
		smb_hc_write(hc, ACPI_SMB_BLOCK_COUNT, length);
		for (i = 0; i < length; ++i)
			smb_hc_write(hc, ACPI_SMB_DATA + i, data[i]);
	}
	smb_hc_write(hc, ACPI_SMB_ADDRESS, address << 1);
	smb_hc_write(hc, ACPI_SMB_PROTOCOL, protocol);
	/*
	 * Wait for completion. Save the status code, data size,
	 * and data into the return package (if required by the protocol).
	 */
	ret = wait_transaction_complete(hc, 1000);
	if (ret || !(protocol & 0x01))
		goto end;
	switch (protocol) {
	case SMBUS_RECEIVE_BYTE:
	case SMBUS_READ_BYTE:
		sz = 1;
		break;
	case SMBUS_READ_WORD:
		sz = 2;
		break;
	case SMBUS_READ_BLOCK:
		if (smb_hc_read(hc, ACPI_SMB_BLOCK_COUNT, &sz)) {
			ret = -EFAULT;
			goto end;
		}
		sz &= 0x1f;
		break;
	}
	for (i = 0; i < sz; ++i)
		smb_hc_read(hc, ACPI_SMB_DATA + i, &data[i]);
      end:
	mutex_unlock(&hc->lock);
	return ret;
}