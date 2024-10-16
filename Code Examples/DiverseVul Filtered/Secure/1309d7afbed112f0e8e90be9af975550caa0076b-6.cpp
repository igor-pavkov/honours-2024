ssize_t tpm_getcap(struct device *dev, __be32 subcap_id, cap_t *cap,
		   const char *desc)
{
	struct tpm_cmd_t tpm_cmd;
	int rc;
	struct tpm_chip *chip = dev_get_drvdata(dev);

	tpm_cmd.header.in = tpm_getcap_header;
	if (subcap_id == CAP_VERSION_1_1 || subcap_id == CAP_VERSION_1_2) {
		tpm_cmd.params.getcap_in.cap = subcap_id;
		/*subcap field not necessary */
		tpm_cmd.params.getcap_in.subcap_size = cpu_to_be32(0);
		tpm_cmd.header.in.length -= cpu_to_be32(sizeof(__be32));
	} else {
		if (subcap_id == TPM_CAP_FLAG_PERM ||
		    subcap_id == TPM_CAP_FLAG_VOL)
			tpm_cmd.params.getcap_in.cap = TPM_CAP_FLAG;
		else
			tpm_cmd.params.getcap_in.cap = TPM_CAP_PROP;
		tpm_cmd.params.getcap_in.subcap_size = cpu_to_be32(4);
		tpm_cmd.params.getcap_in.subcap = subcap_id;
	}
	rc = transmit_cmd(chip, &tpm_cmd, TPM_INTERNAL_RESULT_SIZE, desc);
	if (!rc)
		*cap = tpm_cmd.params.getcap_out.cap;
	return rc;
}