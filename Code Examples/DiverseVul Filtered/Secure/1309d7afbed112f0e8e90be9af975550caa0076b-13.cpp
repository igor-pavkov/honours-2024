ssize_t tpm_show_owned(struct device * dev, struct device_attribute * attr,
			char *buf)
{
	cap_t cap;
	ssize_t rc;

	rc = tpm_getcap(dev, TPM_CAP_PROP_OWNER, &cap,
			 "attempting to determine the owner state");
	if (rc)
		return 0;

	rc = sprintf(buf, "%d\n", cap.owned);
	return rc;
}