static int smbus_alarm(void *context)
{
	struct acpi_smb_hc *hc = context;
	union acpi_smb_status status;
	u8 address;
	if (smb_hc_read(hc, ACPI_SMB_STATUS, &status.raw))
		return 0;
	/* Check if it is only a completion notify */
	if (status.fields.done && status.fields.status == SMBUS_OK) {
		hc->done = true;
		wake_up(&hc->wait);
	}
	if (!status.fields.alarm)
		return 0;
	mutex_lock(&hc->lock);
	smb_hc_read(hc, ACPI_SMB_ALARM_ADDRESS, &address);
	status.fields.alarm = 0;
	smb_hc_write(hc, ACPI_SMB_STATUS, status.raw);
	/* We are only interested in events coming from known devices */
	switch (address >> 1) {
		case ACPI_SBS_CHARGER:
		case ACPI_SBS_MANAGER:
		case ACPI_SBS_BATTERY:
			acpi_os_execute(OSL_NOTIFY_HANDLER,
					acpi_smbus_callback, hc);
		default:;
	}
	mutex_unlock(&hc->lock);
	return 0;
}