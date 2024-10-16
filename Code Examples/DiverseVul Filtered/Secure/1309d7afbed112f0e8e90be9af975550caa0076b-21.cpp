static ssize_t transmit_cmd(struct tpm_chip *chip, struct tpm_cmd_t *cmd,
			    int len, const char *desc)
{
	int err;

	len = tpm_transmit(chip,(u8 *) cmd, len);
	if (len <  0)
		return len;
	if (len == TPM_ERROR_SIZE) {
		err = be32_to_cpu(cmd->header.out.return_code);
		dev_dbg(chip->dev, "A TPM error (%d) occurred %s\n", err, desc);
		return err;
	}
	return 0;
}