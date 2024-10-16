int tpm_pm_suspend(struct device *dev, pm_message_t pm_state)
{
	struct tpm_chip *chip = dev_get_drvdata(dev);
	struct tpm_cmd_t cmd;
	int rc;

	u8 dummy_hash[TPM_DIGEST_SIZE] = { 0 };

	if (chip == NULL)
		return -ENODEV;

	/* for buggy tpm, flush pcrs with extend to selected dummy */
	if (tpm_suspend_pcr) {
		cmd.header.in = pcrextend_header;
		cmd.params.pcrextend_in.pcr_idx = cpu_to_be32(tpm_suspend_pcr);
		memcpy(cmd.params.pcrextend_in.hash, dummy_hash,
		       TPM_DIGEST_SIZE);
		rc = transmit_cmd(chip, &cmd, EXTEND_PCR_RESULT_SIZE,
				  "extending dummy pcr before suspend");
	}

	/* now do the actual savestate */
	cmd.header.in = savestate_header;
	rc = transmit_cmd(chip, &cmd, SAVESTATE_RESULT_SIZE,
			  "sending savestate before suspend");
	return rc;
}