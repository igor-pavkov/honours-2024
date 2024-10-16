ssize_t tpm_show_temp_deactivated(struct device * dev,
				struct device_attribute * attr, char *buf)
{
	cap_t cap;
	ssize_t rc;

	rc = tpm_getcap(dev, TPM_CAP_FLAG_VOL, &cap,
			 "attempting to determine the temporary state");
	if (rc)
		return 0;

	rc = sprintf(buf, "%d\n", cap.stclear_flags.deactivated);
	return rc;
}