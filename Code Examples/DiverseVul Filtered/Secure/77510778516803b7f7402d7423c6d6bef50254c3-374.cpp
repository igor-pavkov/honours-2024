static u32 ctrn_u32_to_index(u32 val)
{
	if (!val) return 0;
	if (val<=255) return 1;
	if (val<=65535) return 2;
	return 3;
}