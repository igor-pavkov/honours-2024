ssize_t tpm_show_enabled(struct device * dev, struct device_attribute * attr,
			char *buf)
{
	cap_t cap;
	ssize_t rc;

	rc = tpm_getcap(dev, TPM_CAP_FLAG_PERM, &cap,
			 "attempting to determine the permanent enabled state");
	if (rc)
		return 0;

	rc = sprintf(buf, "%d\n", !cap.perm_flags.disable);
	return rc;
}