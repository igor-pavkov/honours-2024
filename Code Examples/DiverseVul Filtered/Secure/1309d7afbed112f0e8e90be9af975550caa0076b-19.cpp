void tpm_continue_selftest(struct tpm_chip *chip)
{
	u8 data[] = {
		0, 193,			/* TPM_TAG_RQU_COMMAND */
		0, 0, 0, 10,		/* length */
		0, 0, 0, 83,		/* TPM_ORD_GetCapability */
	};

	tpm_transmit(chip, data, sizeof(data));
}