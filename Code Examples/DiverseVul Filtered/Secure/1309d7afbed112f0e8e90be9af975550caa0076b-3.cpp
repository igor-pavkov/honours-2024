int tpm_send(u32 chip_num, void *cmd, size_t buflen)
{
	struct tpm_chip *chip;
	int rc;

	chip = tpm_chip_find_get(chip_num);
	if (chip == NULL)
		return -ENODEV;

	rc = transmit_cmd(chip, cmd, buflen, "attempting tpm_cmd");

	tpm_chip_put(chip);
	return rc;
}