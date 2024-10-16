static int possible_passthrough_msr_slot(u32 msr)
{
	u32 i;

	for (i = 0; i < ARRAY_SIZE(vmx_possible_passthrough_msrs); i++)
		if (vmx_possible_passthrough_msrs[i] == msr)
			return i;

	return -ENOENT;
}