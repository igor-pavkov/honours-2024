static int wait_transaction_complete(struct acpi_smb_hc *hc, int timeout)
{
	if (wait_event_timeout(hc->wait, hc->done, msecs_to_jiffies(timeout)))
		return 0;
	return -ETIME;
}