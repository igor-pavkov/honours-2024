int tpm_pcr_read(u32 chip_num, int pcr_idx, u8 *res_buf)
{
	struct tpm_chip *chip;
	int rc;

	chip = tpm_chip_find_get(chip_num);
	if (chip == NULL)
		return -ENODEV;
	rc = __tpm_pcr_read(chip, pcr_idx, res_buf);
	tpm_chip_put(chip);
	return rc;
}