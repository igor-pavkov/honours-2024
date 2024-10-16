int __tpm_pcr_read(struct tpm_chip *chip, int pcr_idx, u8 *res_buf)
{
	int rc;
	struct tpm_cmd_t cmd;

	cmd.header.in = pcrread_header;
	cmd.params.pcrread_in.pcr_idx = cpu_to_be32(pcr_idx);
	rc = transmit_cmd(chip, &cmd, READ_PCR_RESULT_SIZE,
			  "attempting to read a pcr value");

	if (rc == 0)
		memcpy(res_buf, cmd.params.pcrread_out.pcr_result,
		       TPM_DIGEST_SIZE);
	return rc;
}