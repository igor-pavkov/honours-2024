unsigned long tpm_calc_ordinal_duration(struct tpm_chip *chip,
					   u32 ordinal)
{
	int duration_idx = TPM_UNDEFINED;
	int duration = 0;

	if (ordinal < TPM_MAX_ORDINAL)
		duration_idx = tpm_ordinal_duration[ordinal];
	else if ((ordinal & TPM_PROTECTED_ORDINAL_MASK) <
		 TPM_MAX_PROTECTED_ORDINAL)
		duration_idx =
		    tpm_protected_ordinal_duration[ordinal &
						   TPM_PROTECTED_ORDINAL_MASK];

	if (duration_idx != TPM_UNDEFINED)
		duration = chip->vendor.duration[duration_idx];
	if (duration <= 0)
		return 2 * 60 * HZ;
	else
		return duration;
}