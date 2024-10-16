u32 gf_isom_ctrn_field_size_bits(u32 field_idx)
{
	if (field_idx==3) return 32;
	return field_idx*8;
}