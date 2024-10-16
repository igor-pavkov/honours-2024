static u32 ctrn_sample_flags_to_index(u32 val)
{
	if (!val) return 0;
	if (val & 0x0000FFFF)
		return 3;
	if (val & 0x00FF0000)
		return 2;
	return 1;
}