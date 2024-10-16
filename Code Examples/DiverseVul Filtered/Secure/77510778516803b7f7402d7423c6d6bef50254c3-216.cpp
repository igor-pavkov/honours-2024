static u32 ctrn_s32_to_index(s32 val)
{
	if (!val) return 0;
	if (ABS(val)<=127) return 1;
	if (ABS(val)<=32767) return 2;
	return 3;
}